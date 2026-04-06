#pragma once

#include "arch/x86_64/def.h"
#include "arch/x86_64/mm/paging.h"
#include "bootstub.h"
#include "misc/helpers.h"
#include "mm/vmm/vmm.h"

void    kheap_init(page_table_t *pt, vaddr_t begin, size_t pages);
void*   kheap_alloc(size_t pages);
void    kheap_free(void* addr);
void    kheap_debug(void);
void*   kmmalloc(size_t pages, vmm_area_state_t state);

static inline void *kmalloc(size_t bytes)
{  
    size_t pages = align_up(bytes, PAGE_SIZE) / PAGE_SIZE;
    return kheap_alloc(pages);
}


static inline void* kheap_alloc_with_bytes(size_t bytes)
{
    size_t pages = align_up(bytes, PAGE_SIZE) / PAGE_SIZE;
    return kheap_alloc(pages);
}
