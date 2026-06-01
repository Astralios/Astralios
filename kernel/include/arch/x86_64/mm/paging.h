#pragma once

#include <kernel.h>
#include <stdint.h>

typedef enum page_flags_t
{
    PAGE_FLAG_PRESENT           = 1 << 0,
    PAGE_FLAG_READ_WRITE        = 1 << 1,
    PAGE_FLAG_USER_SUPERVISOR   = 1 << 2,
    PAGE_FLAG_WRITE_THROUGH     = 1 << 3,
    PAGE_FLAG_CACHE_DISABLE     = 1 << 4,
    PAGE_FLAG_ACCESSED          = 1 << 5,
    PAGE_FLAG_DIRTY             = 1 << 6,
    PAGE_FLAG_PAT               = 1 << 7,
    PAGE_FLAG_GLOBAL            = 1 << 8,
    PAGE_FLAG_COW               = 1 << 9,
} page_flags_t;

typedef uint64_t page_entry_t;
typedef struct page_table_t {
    page_entry_t entries[512];
} page_table_t;

#define PAGE_PHYSICAL_ADDRESS_MASK  0xfffffffff000

void map_to_pt(page_table_t *pt, paddr_t paddr, vaddr_t vaddr, page_flags_t flags);
void unmap_from_pt(page_table_t *pt, vaddr_t vaddr);
void map_prange_to_pt(page_table_t *pt, paddr_t from, paddr_t to, vaddr_t at, page_flags_t flags);
void unmap_range_from_pt(page_table_t *pt, vaddr_t from, vaddr_t to);
void map_vrange_to_pt(page_table_t *pt, vaddr_t from, vaddr_t to, paddr_t at, page_flags_t flags);
void map_kernel_to_pt(page_table_t *pt);
void map_memmap_to_pt(page_table_t *pt);

