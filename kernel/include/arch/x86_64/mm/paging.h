#pragma once

#include <stdint.h>
#include "arch/x86_64/def.h"

typedef enum page_flags_t
{
    PAGE_P  = 1 << 0,
    PAGE_RW = 1 << 1,
    PAGE_US = 1 << 2,
    PAGE_WT = 1 << 3,
    PAGE_CD = 1 << 4,
    PAGE_A  = 1 << 5,
    PAGE_D  = 1 << 6,
    PAGE_S  = 1 << 7,
    PAGE_G  = 1 << 8,
    PAGE_PAT = 1 << 12,
} page_flags_t;

typedef uint64_t page_entry_t;
typedef struct page_table_t {
    page_entry_t entries[512];
} page_table_t;

#define PAGE_PHYSICAL_ADDRESS_MASK 0xfffffffff000
#define PAGE_SIZE 4096

void map_to_pt(page_table_t *pt, paddr_t paddr, vaddr_t vaddr, page_flags_t flags);
void unmap_from_pt(page_table_t *pt, vaddr_t vaddr);
void map_prange_to_pt(page_table_t *pt, paddr_t from, paddr_t to, vaddr_t at, page_flags_t flags);
void unmap_range_from_pt(page_table_t *pt, vaddr_t from, vaddr_t to);
void map_vrange_to_pt(page_table_t *pt, vaddr_t from, vaddr_t to, paddr_t at, page_flags_t flags);
void map_kernel_to_pt(page_table_t *pt);
void map_memmap_to_pt(page_table_t *pt);

