#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

#include "arch/x86_64/mem/paging.h"
#include "arch/x86_64/def.h"
#include "arch/x86_64/cpu.h"
#include "misc/debug.h"
#include "misc/helpers.h"
#include "string.h"
#include "bootstub.h"
#include "mem/pmm.h"
#include "arch/x86_64/cpu.h"

extern kernel_params_t *kernel_params;

static void set_page_entry(page_entry_t *entry)
{
    if (!(*entry & PAGE_P))
    {
        *entry = 0;
        *entry |= (PAGE_P | PAGE_RW | PAGE_US);
        *entry |= pmm_alloc(1) & PAGE_PHYSICAL_ADDRESS_MASK;
        memset((void*)to_vaddr((*entry & PAGE_PHYSICAL_ADDRESS_MASK)), 0, PAGE_SIZE);
    }
}

void map_to_pt(page_table_t *pt, paddr_t paddr, vaddr_t vaddr, page_flags_t flags)
{
    size_t pt4_idx = (vaddr >> 39) & 0x1FF;
    size_t pt3_idx = (vaddr >> 30) & 0x1FF;
    size_t pt2_idx = (vaddr >> 21) & 0x1FF;
    size_t pt1_idx = (vaddr >> 12) & 0x1FF;

    set_page_entry(&pt->entries[pt4_idx]);

    page_table_t *pt3 = paddr_ptr(pt->entries[pt4_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    set_page_entry(&pt3->entries[pt3_idx]);

    page_table_t *pt2 = paddr_ptr(pt3->entries[pt3_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    set_page_entry(&pt2->entries[pt2_idx]);

    page_table_t *pt1 = paddr_ptr(pt2->entries[pt2_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    pt1->entries[pt1_idx] = (paddr & PAGE_PHYSICAL_ADDRESS_MASK) | (flags & 0xFFF);
}

void unmap_from_pt(page_table_t *pt, vaddr_t vaddr)
{
    size_t pt4_idx = (vaddr >> 39) & 0x1FF;
    size_t pt3_idx = (vaddr >> 30) & 0x1FF;
    size_t pt2_idx = (vaddr >> 21) & 0x1FF;
    size_t pt1_idx = (vaddr >> 12) & 0x1FF;
    
    page_table_t *pt3 = paddr_ptr(pt->entries[pt4_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    page_table_t *pt2 = paddr_ptr(pt3->entries[pt3_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    page_table_t *pt1 = paddr_ptr(pt2->entries[pt2_idx] & PAGE_PHYSICAL_ADDRESS_MASK);
    
    pmm_free(pt1->entries[pt1_idx] & PAGE_PHYSICAL_ADDRESS_MASK, PAGE_SIZE / PAGE_SIZE);
    
    pt1->entries[pt1_idx] = 0;
    invplg((uint64_t)pt); 
}

void map_prange_to_pt(page_table_t *pt, paddr_t from, paddr_t to, vaddr_t at, page_flags_t flags)
{
    uint64_t pages  = (to - from) / PAGE_SIZE;
    for (uint64_t i = 0; i < pages; i++)
    {
        map_to_pt(
            pt, 
            from + i * PAGE_SIZE, 
            at + i * PAGE_SIZE, 
            flags
        );
    }
}

void map_vrange_to_pt(page_table_t *pt, vaddr_t from, vaddr_t to, paddr_t at, page_flags_t flags)
{
    uint64_t pages = (to - from) / PAGE_SIZE;
    for (uint64_t i = 0; i < pages; i++)
    {
        map_to_pt(
                pt, 
                at + i * PAGE_SIZE, 
                from + i * PAGE_SIZE,
                flags
            );
    }
}

void unmap_range_from_pt(page_table_t *pt, vaddr_t from, vaddr_t to)
{
    uint64_t pages  = (to - from) / PAGE_SIZE;
    for (size_t i = 0; i < pages; i++)
    {
        unmap_from_pt(pt, from + i * PAGE_SIZE);
    }
}

static void map_kernel_sections_to_pt(page_table_t *pt, vaddr_t sec_start, vaddr_t sec_end, page_flags_t flags)
{
    // sec_start = ALIGN_DOWN(sec_start, PAGE_SIZE);
    // sec_end   = ALIGN_UP(sec_end, PAGE_SIZE);

    size_t offset = sec_start - (vaddr_t)kernel_params->kernel_addr.virtual_base;
    paddr_t kernel_paddr = kernel_params->kernel_addr.physical_base;
    map_vrange_to_pt(pt, sec_start, sec_end, kernel_paddr + offset, flags);
}

void map_kernel_to_pt(page_table_t *pt)
{
    extern char text_start[];
    extern char text_end[];
    
    extern char rodata_start[];
    extern char rodata_end[];
    
    extern char mut_data_start[];
    extern char mut_data_end[];
   
    map_kernel_sections_to_pt(pt, (vaddr_t)text_start, (vaddr_t)text_end, PAGE_P);
    map_kernel_sections_to_pt(pt, (vaddr_t)rodata_start, (vaddr_t)rodata_end, PAGE_P);
    map_kernel_sections_to_pt(pt, (vaddr_t)mut_data_start, (vaddr_t)mut_data_end, PAGE_P | PAGE_RW);
}

void map_memmap_to_pt(page_table_t *pt)
{
    for (size_t i = 0; i < kernel_params->memmap.entry_count; i++)
    {
        memmap_entry_t entry = kernel_params->memmap.entries[i];
        if (entry.type == MEMMAP_FRAMEBUFFER || entry.type == MEMMAP_USABLE || entry.type == MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            uint64_t base = ALIGN_UP(entry.base, PAGE_SIZE);
            size_t len = ALIGN_DOWN(entry.length, PAGE_SIZE);
            
            if (entry.type == MEMMAP_FRAMEBUFFER)
            {
                map_prange_to_pt(pt, base, base + len, to_vaddr(base), PAGE_P | PAGE_RW | PAGE_AT | PAGE_WT);
            } else {
                map_prange_to_pt(pt, base, base + len, to_vaddr(base), PAGE_P | PAGE_RW);
            }
        }
    }
}

