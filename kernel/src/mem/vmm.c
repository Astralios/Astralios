#include "mem/vmm.h"
#include "arch/x86_64/cpu.h"
#include "mem/pmm.h"
#include "misc/debug.h"
#include "misc/helpers.h"
#include "vendor/hm.h"
#include <stdarg.h>
#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#include "arch/x86_64/mem/paging.h"
#endif

extern page_table_t *pml4;

static const char* vmm_region_state_str[VMM_REGION_STATE_COUNT] = {
    [FREE] = "free",
    [ALLOCATED] = "allocated",
    [LAZILY_ALLOCATED] = "lazily allocated",
};

static const char* vmm_flag_str[VMM_FLAGS_COUNT] = {
    [VMM_WRITEABLE] = "Writable",
    [VMM_EXECUTABLE] = "Executable",
    [VMM_USER_ACCESSIBLE] = "User Accessible",
    [VMM_UNCACHEABLE] = "Uncacheable",
    [VMM_FRAMEBUFFER_CACHED] = "Framebuffer Cached",
    [VMM_ZEROED] = "Zeroed",
};

const char *vmm_region_state_to_str(vmm_region_state_t state)
{
    return vmm_region_state_str[state];
}

const char *vmm_flag_to_str(vmm_flag_t flag)
{
    return vmm_flag_str[flag];
}

void vmm_debug(vmm_debug_mode_t mode, ...)
{
    va_list args;
    va_start(args, mode);

    switch (mode) {
        case DEBUG_VMM_REGION_PRINT:
        {
            vmm_region_t *list = va_arg(args, vmm_region_t*);
            vmm_region_t *curr = list;
            while (curr)
            {
                srdebug(vmm_debug, "name: %s; addr: %x; flags: %x; pages: %zu; state: %s; next: %x; prev: %x", curr->name, curr->addr, curr->flags, curr->pages, vmm_region_state_str[curr->state], curr->next, curr->prev);
                curr = curr->next;
            }
            break;
        }
    }

    va_end(args);
}

vaddr_t vmm_alloc(vmm_t *vmm, char *name, size_t len, int flags)
{
    if (len == 0) return 0;
    if (name == NULL) name = "Unnamed";
    
    size_t pages = ALIGN_UP(len, PAGE_SIZE) / PAGE_SIZE;
    vmm_region_t *curr = vmm->regions;
    while (curr)
    {
        if (curr->pages >= pages && curr->state == FREE)
        {
            if (curr->pages > pages)
            {
                vmm_region_t *new_region = paddr_ptr(pmm_alloc_with_bytes(sizeof(vmm_region_t)));

                new_region->flags = curr->flags;
                new_region->pages = curr->pages - pages;
                new_region->name = curr->name;
                new_region->state = curr->state;
                new_region->next = curr->next;
                new_region->prev = curr;
                new_region->addr = curr->addr + ALIGN_UP(len, PAGE_SIZE);

                curr->next = new_region;
                curr->flags = flags;
                curr->state = ALLOCATED;
                curr->name = name;
                curr->pages = pages;
      
                for (size_t i = 0; i < curr->pages; i++)
                {
                    map_to_pt(vmm->pt, pmm_alloc(1), curr->addr + i * PAGE_SIZE, flags);
                }
         
                return curr->addr;
            } else if (curr->pages == pages)
            {
                curr->state = ALLOCATED;
                curr->flags = flags;
                curr->name = name;
                for (size_t i = 0; i < curr->pages; i++)
                {
                    map_to_pt(vmm->pt, pmm_alloc(1), curr->addr + i * PAGE_SIZE, flags);
                }

                return curr->addr;
            }
        }
        curr = curr->next;
    }
    return 0;
}

void vmm_free(vmm_t *vmm, vaddr_t vaddr)
{
    if (vaddr == 0) return;
    
    vmm_region_t *curr = vmm->regions;
    while (curr)
    {
        if (curr->addr == vaddr) break;
        curr = curr->next;
    }
    if (curr == NULL) return;
    curr->state = FREE;
    
    if (curr->prev && curr->prev->state == FREE)
    {
        curr->prev->pages += curr->pages;
        curr->prev->next = curr->next;
        if (curr->next) curr->next->prev = curr->prev;
        pmm_free_with_bytes(to_paddr(((paddr_t)curr)), sizeof(vmm_region_t));
    }

    if (curr->next && curr->next->state == FREE)
    {
        curr->pages += curr->next->pages;
        curr->next = curr->next->next;
        if (curr->next->next) curr->next->next->prev = curr;
        pmm_free_with_bytes((paddr_t)curr->next, sizeof(vmm_region_t));
    }

    pmm_free(to_paddr(curr->addr), curr->pages);

    for (size_t i = 0; i < curr->pages; i++)
    {
        unmap_from_pt(vmm->pt, curr->addr + i * PAGE_SIZE);
        invplg(curr->addr);
    }
}
