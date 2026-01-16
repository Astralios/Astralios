#pragma once

#include <stdint.h>
#include "arch/x86_64/def.h"

#define PAGE_PRESENT (1 << 0)
#define PAGE_READ_WRITE (1 << 1)
#define PAGE_USER_SUPERVISOR (1 << 2)
#define PAGE_WRITE_THROUGH (1 << 3)
#define PAGE_CACHE_DISABLE (1 << 4)
#define PAGE_ACCESS (1 << 5)
#define PAGE_DIRTY (1 << 6)
#define PAGE_PAGE_SIZE (1 << 7)
#define PAGE_GLOBAL (1 << 8)
#define PAGE_ATTRIBUTE_TABLE (1 << 12)

#define PAGE_PHYSICAL_ADDRESS_MASK 0xfffffffff000

#define PAGE_SIZE 4096

extern char section_text_begin[];
extern char section_text_end[];
extern char section_const_data_begin[];
extern char section_const_data_end[];
extern char section_mut_data_begin[];
extern char section_mut_data_end[];

typedef uint64_t page_entry_t;
typedef struct page_table_t
{
    page_entry_t entries[512];
} page_table_t;

void paging_init(void);
void map_page_table(phys_addr_t phys_addr, virt_addr_t virtual_addr, uint16_t flags);
void unmap_page_table(virt_addr_t virt_addr);
