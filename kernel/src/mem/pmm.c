#include "mem/pmm.h"
#include "arch/x86_64/mem/paging.h"
#include "bootstub.h"
#include "devices/serial.h"
#include "misc/debug.h"
#include "string.h"
#include "vendor/bitmap.h"
#include "misc/helpers.h"
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

// Temporarily here
const char* memmap_entries_type_str[MEMMAP_ENTRY_TYPE_COUNT] = {
    [MEMMAP_RESERVED] = "Reserved",
    [MEMMAP_EXECUTABLE_AND_MODULES] = "Executable and Modules",
    [MEMMAP_USABLE] = "Usable",
    [MEMMAP_ACPI_NVS] = "ACPI NVS",
    [MEMMAP_ACPI_RECLAIMABLE] = "ACPI Reclaimable",
    [MEMMAP_BOOTLOADER_RECLAIMABLE] = "Bootloader Reclaimable",
    [MEMMAP_BAD_MEMORY] = "Bad Memory",
    [MEMMAP_FRAMEBUFFER] = "Framebuffer",
    [MEMMAP_UNKNOWN] = "Unknown",
};

void pmm_init(void)
{ 
    memmap_t memmap = kernel_params->memmap;
    for (size_t i = 0; i < memmap.entry_count; i++)
    {
        memmap_entry_t entry = memmap.entries[i];
        paddr_t base = ALIGN_UP(entry.base, PAGE_SIZE);
        size_t  len = ALIGN_DOWN(entry.length, PAGE_SIZE);
        size_t  entry_pages = len / PAGE_SIZE;

        srdebug(pmm_init, "base: %x, len: %zu (bytes), type: %s", base, len, memmap_entries_type_str[entry.type]);
            
        if (entry.type == MEMMAP_USABLE)
        {  
            if (entry_pages > 1)
            {
                size_t free_pages = entry_pages - (8 * entry_pages + 7) / 9;
                size_t bitmap_size = (free_pages + 7) / 8;

                pmm_regions_full_size_in_pages += free_pages;
                pmm_free_mem_amount += free_pages;
            
                pmm_region_t *region = paddr_ptr(base);
                
                region->first_free_idx = 0;
                region->bitmap.size = free_pages;
                region->pages_available = free_pages;
                    region->free_paddr = base + ALIGN_UP(sizeof(*region) + (bitmap_size + 7) / 8, PAGE_SIZE);
                
                region->next = region_list;
                memset(region->bitmap.map, 0, (bitmap_size + 7) / 8);

                region_list = region;
            }
        }
    }

    srdebug(pmm_init, "Initialized");
}

paddr_t pmm_alloc(size_t pages_count)
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
                srdebug(pmm_alloc, "Found free memory at idx within %x region: %zu", curr, idx);
                if (idx == curr->first_free_idx) curr->first_free_idx += pages_count;
                bitmap_set_range(&curr->bitmap, idx, pages_count); 
                
                curr->pages_available -= pages_count;
                pmm_free_mem_amount -= pages_count;

                paddr_t addr = (curr->free_paddr) + idx * PAGE_SIZE;
                srdebug(pmm_alloc, "Allocated at: %x", addr);
                return addr;
            }
        }
        curr = curr->next;
    }
    return 0;
}

paddr_t pmm_alloc_with_bytes(size_t bytes)
{
    size_t pages_count = ALIGN_UP(bytes, PAGE_SIZE) / PAGE_SIZE;
    return pmm_alloc(pages_count);
}

void pmm_free(paddr_t addr, size_t pages_count)
{
    pmm_region_t *curr = region_list;
    
    while (curr)
    {
        paddr_t lower_bound = to_paddr(curr);
        paddr_t higher_bound = to_paddr(curr) + sizeof(*curr) + curr->bitmap.size * PAGE_SIZE;   
        if (lower_bound <= addr && addr < higher_bound) break;
        curr = curr->next;
    }
    if (!curr) return;
    
    size_t idx = (addr - (to_paddr(curr->bitmap.map)) - (curr->bitmap.size + 7) / 8) / PAGE_SIZE;
    if (idx < curr->first_free_idx) curr->first_free_idx = idx;
    bitmap_unset_range(&curr->bitmap, idx, pages_count);
}

void pmm_free_with_bytes(paddr_t addr, size_t bytes) {
    size_t pages_count = ALIGN_UP(bytes, PAGE_SIZE) / PAGE_SIZE;
    pmm_free(addr, pages_count);
}
