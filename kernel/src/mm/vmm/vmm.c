#include "mm/vmm/vmm.h"
#include "arch/x86_64/cpu/cpu.h"
#include "arch/x86_64/def.h"
#include "arch/x86_64/mm/paging.h"
#include "misc/debug.h"
#include "mm/pmm/pmm.h"
#include "vendor/list.h"
#include <mm/vmm/misc.h>
#include <string.h>

void vmm_init(vmm_t *vmm, page_table_t *pt, vaddr_t addr, size_t num_pages)
{
    vmm->pt = pt;
 
    vmm_area_t *area = paddr_ptr(pmm_alloc(sizeof(vmm_area_t)));
    area->addr = addr;
    area->num_pages = num_pages;
    area->state = FREE;
    area->page_flags = 0;
    

    list_init(&vmm->area_list_head);
    list_append(&vmm->area_list_head, &area->area_list);
}

static vmm_area_t *vmm_find_area(vmm_t *vmm, vaddr_t addr)
{
    foreach (node, &vmm->area_list_head) {
        vmm_area_t *curr = container_of(node, vmm_area_t, area_list);
        if (curr->addr == addr)
            return curr;
    }
    return NULL;  
}

static vmm_area_t* vmm_find_sufficiently_sized_area(vmm_t *vmm, size_t num_pages)
{
    foreach (node, &vmm->area_list_head)
    {
        vmm_area_t *curr = container_of(node, vmm_area_t, area_list);
        if (curr->num_pages >= num_pages && curr->state == FREE)
            return curr;
    }
    return NULL;
}

static void vmm_map_area(vmm_t *vmm, vmm_area_t *area)
{
    for (size_t i = 0; i < area->num_pages; ++i)
    {
        map_to_pt(vmm->pt, pmm_palloc(1), area->addr + i * PAGE_SIZE, area->page_flags | PAGE_FLAG_PRESENT);
    }
}

static void vmm_area_split(vmm_area_t *area, size_t num_pages)
{
    if (num_pages >= area->num_pages)
        return;
    vmm_area_t *other_area = paddr_ptr(pmm_alloc(sizeof(vmm_area_t)));
    other_area->addr = area->addr + num_pages * PAGE_SIZE;
    other_area->num_pages = area->num_pages - num_pages;
    other_area->page_flags = area->page_flags;
    other_area->state = area->state;

    area->num_pages = num_pages;

    list_insert(area->area_list.next, &other_area->area_list);
}

vaddr_t vmm_palloc(vmm_t *vmm, size_t num_pages, page_flags_t page_flags)
{
    if (num_pages == 0) return 0;

    vmm_area_t *area = vmm_find_sufficiently_sized_area(vmm, num_pages);
    if (!area)
      return 0;

    if (area->num_pages > num_pages)
      vmm_area_split(area, num_pages);

    area->state = ALLOCATED;
    area->page_flags = page_flags;
    vmm_map_area(vmm, area);

    return area->addr;
}

static void vmm_unmap_area(vmm_t *vmm, vmm_area_t *area)
{
  for (size_t i = 0; i < area->num_pages; ++i) {
    unmap_from_pt(vmm->pt, area->addr + i * PAGE_SIZE);
    invplg(area->addr + i * PAGE_SIZE);
  }
}

static void vmm_area_try_merge_with_neighbors(vmm_area_t *area)
{
    vmm_area_t *prev_area = container_of(area->area_list.prev, vmm_area_t, area_list);
    vmm_area_t *next_area = container_of(area->area_list.next, vmm_area_t, area_list);

    if (prev_area && prev_area->state == FREE)
    {
        area->addr = prev_area->addr;
        area->num_pages += prev_area->num_pages;
        list_remove(&prev_area->area_list);
        pmm_free(to_paddr(prev_area), sizeof(vmm_area_t));
    }

    if (next_area && next_area->state == FREE)
    {
        area->num_pages += next_area->num_pages;
        list_remove(&next_area->area_list);
        pmm_free(to_paddr(next_area), sizeof(vmm_area_t));
    }
}

void vmm_free(vmm_t *vmm, vaddr_t addr)
{
    if (addr == 0)
        return;

    vmm_area_t *area = vmm_find_area(vmm, addr);
    area->state = FREE;
    area->page_flags = 0;

    vmm_unmap_area(vmm, area);
    vmm_area_try_merge_with_neighbors(area);
}
