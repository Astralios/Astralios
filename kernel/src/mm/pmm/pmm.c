#include "bootstub.h"
#include <string.h>
#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#endif

#include "mm/pmm/pmm.h"
#include "misc/helpers.h"
#include "misc/debug.h"

extern kernel_params_t *kernel_params;

static pmm_area_t *area_list  = NULL;

void pmm_init(void)
{
    memmap_t memmap = kernel_params->memmap;
    for (size_t i = 0; i < memmap.entry_count; ++i)
    {
        memmap_entry_t entry = memmap.entries[i];
        paddr_t base = ALIGN_UP(entry.base, PAGE_SIZE);
        size_t  len  = ALIGN_DOWN(entry.length, PAGE_SIZE);
        size_t  num_pages = len / PAGE_SIZE;
    
        if (entry.type == MEMMAP_USABLE)
        {
            if (num_pages > 1)
            {
                // Formula I derived
                size_t num_free_pages = (8 * (num_pages * 4096 - 4095) - 7) / (4096 * 8 + 1) + 1;
                size_t bitmap_size = (num_pages + 7) / 8;
                
                pmm_area_t *area = paddr_ptr(base);
                
                area->free_idx_hint = 0;
                area->bitmap.size = num_free_pages;
                area->num_pages = num_free_pages;
                area->addr = base + ALIGN_UP(sizeof(pmm_area_t) + bitmap_size, PAGE_SIZE);
                area->next = area_list;

                memset(area->bitmap.map, 0, bitmap_size);

                area_list = area;
            } else {

            }
        }
    }

    info(pmm_init, "Initialized!"); 
}

paddr_t pmm_alloc(size_t size)
{
}

paddr_t pmm_palloc(size_t num_pages)
{
}

void pmm_free(paddr_t addr, size_t size)
{
}

void pmm_pfree(paddr_t addr, size_t num_pages)
{
}
