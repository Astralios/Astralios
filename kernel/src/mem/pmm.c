#include "mem/pmm.h"
#include "bootstub.h"
#include "devices/serial.h"
#include "misc/debug.h"
#include "string.h"
#include "vendor/bitmap.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#endif

extern kernel_params_t *kernel_params;
static pmm_region_t *region_list = NULL;

static size_t pmm_regions_full_size_in_pages = 0;
static size_t pmm_free_mem_amount = 0;

void pmm_debug(pmm_debug_mode_t mode, ...)
{
    va_list args;
    va_start(args, mode);

    switch (mode)
    {
    case DEBUG_PMM_REGIONS: {
        pmm_region_t *curr = region_list;
        while (curr)
        {
            frdebug("PMM_DEBUG", "%zu pages_count; %zu pages_available; %x next;", curr->bitmap.size, curr->pages_available, curr->next);
            curr = curr->next;
        }
        break;
    }
    case DEBUG_PMM_FREE_MEM_SIZE: {
        frdebug("PMM_DEBUG", "pmm free mem size: %zu (pages)", pmm_free_mem_amount);
        break;
    }   
    case DEBUG_PMM_REGIONS_FULL_SIZE: {
        frdebug("PMM_DEBUG", "pmm regions full size: %zu (pages)", pmm_regions_full_size_in_pages);
        break;
    }
    case DEBUG_PMM_REGION_BITMAP: {
        pmm_region_t *region = va_arg(args, pmm_region_t*);
        bitmap_t* bitmap = &region->bitmap;
        for (size_t i = 0; i < bitmap->size; i++)
        {
            if (bitmap_bit(bitmap, i) & 0b1) srput('1');
            else srput('0');
        }
        srput('\n');
        break;
    }
    default:
        break;
    }

    va_end(args);
}

void pmm_init(void)
{ 
    memmap_t memmap = kernel_params->memmap;
    for (size_t i = 0; i < memmap.entry_count; i++)
    {
        memmap_entry_t entry = memmap.entries[i];
        if (entry.type == MEMMAP_USABLE)
        {
            phys_addr_t base = entry.base;
            size_t len = entry.length;
            size_t entry_pages = len / PAGE_SIZE;
            
            if (entry_pages > 1)
            {
                size_t free_pages = entry_pages - (8 * entry_pages + 7) / 9;
                size_t bitmap_size = (free_pages + 7) / 8;

                pmm_regions_full_size_in_pages += free_pages;
                pmm_free_mem_amount += free_pages;
            
                pmm_region_t *region = (void*)to_vaddr(base);
                region->first_free_idx = 0;
                region->bitmap.size = free_pages;
                region->pages_available = free_pages;
                region->next = region_list;
                memset(region->bitmap.map, 0, bitmap_size);

                region_list = region;
            }
        }
    }

    srdebug(pmm_init, "Initialized");
}

phys_addr_t pmm_alloc(size_t pages_count)
{
    if (pages_count == 0) return 0;

    pmm_region_t *curr = region_list;
    while (curr)
    {
        if (curr->pages_available >= pages_count)
        {
            size_t idx = bitmap_find_contiguous_range_from(&curr->bitmap, curr->first_free_idx, pages_count);
            if (idx != (size_t)(-1))
            {
                if (idx == curr->first_free_idx) curr->first_free_idx += pages_count;
                bitmap_set_range(&curr->bitmap, idx, pages_count); 
                
                curr->pages_available -= pages_count;
                pmm_free_mem_amount -= pages_count;

                virt_addr_t vaddr = cast_vaddr(curr->bitmap.map) + (curr->bitmap.size + 7) / 8 + idx * PAGE_SIZE;
                phys_addr_t addr = to_paddr(vaddr);
                return addr;
            }
        }
        curr = curr->next;
    }
    return 0;
}

void pmm_free(phys_addr_t addr, size_t pages_count)
{
    pmm_region_t *curr = region_list;
    
    while (curr)
    {
        phys_addr_t lower_bound = to_paddr(curr);
        phys_addr_t higher_bound = to_paddr(curr) + sizeof(*curr) + curr->bitmap.size * PAGE_SIZE;   
        if (lower_bound <= addr && addr < higher_bound) break;
        curr = curr->next;
    }
    if (!curr) return;
    
    size_t idx = (addr - (to_paddr(curr->bitmap.map)) - (curr->bitmap.size + 7) / 8) / PAGE_SIZE;
    if (idx < curr->first_free_idx) curr->first_free_idx = idx;
    bitmap_unset_range(&curr->bitmap, idx, pages_count);
}

