#include "kernel.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <bootstub.h>

#include <arch/x86_64/mm/paging.h>
#include <arch/x86_64/def.h>
#include <arch/x86_64/cpu/cpu.h>

#include <misc/helpers.h>
#include <mm/pmm/pmm.h>

extern krnl_ctx_t krnl_ctx;

static void set_page_entry(page_entry_t *entry)
{
    if (!(*entry & PAGE_FLAG_PRESENT))
    {
        *entry = 0;
        *entry |= (PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE | PAGE_FLAG_USER_SUPERVISOR);
        *entry |= pmm_palloc(1) & PAGE_PHYSICAL_ADDRESS_MASK;
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
    size_t offset = sec_start - (vaddr_t)krnl_ctx.bootloader_ctx->krnl_map.virtual_base;
    paddr_t kernel_paddr = krnl_ctx.bootloader_ctx->krnl_map.physical_base;
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
   
    map_kernel_sections_to_pt(pt, (vaddr_t)text_start, (vaddr_t)text_end, PAGE_FLAG_PRESENT);
    map_kernel_sections_to_pt(pt, (vaddr_t)rodata_start, (vaddr_t)rodata_end, PAGE_FLAG_PRESENT);
    map_kernel_sections_to_pt(pt, (vaddr_t)mut_data_start, (vaddr_t)mut_data_end, PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE);
}

vaddr_t hhdm_end = 0;

void map_memmap_to_pt(page_table_t *pt)
{
    for (size_t i = 0; i < krnl_ctx.bootloader_ctx->memmap.num_entries; i++)
    {
        memmap_entry_t entry = krnl_ctx.bootloader_ctx->memmap.entries[i];
        if (
            entry.type == MEMMAP_FRAMEBUFFER || 
            entry.type == MEMMAP_USABLE || 
            entry.type == MEMMAP_BOOTLOADER_RECLAIMABLE || 
            entry.type == MEMMAP_EXECUTABLE_AND_MODULES || 
            entry.type == MEMMAP_ACPI_NVS || 
            entry.type == MEMMAP_ACPI_RECLAIMABLE || 
            entry.type == MEMMAP_RESERVED_MAPPED
        ) {
            uint64_t base = align_up(entry.base, PAGE_SIZE);
            size_t len = align_down(entry.length, PAGE_SIZE);
            vaddr_t end = to_vaddr(base + len);
            if (end > hhdm_end) hhdm_end = end;
            if (entry.type == MEMMAP_FRAMEBUFFER)
            {
                map_prange_to_pt(pt, base, base + len, to_vaddr(base), PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE | PAGE_FLAG_PAT | PAGE_FLAG_WRITE_THROUGH);
            } else {
                map_prange_to_pt(pt, base, base + len, to_vaddr(base), PAGE_FLAG_PRESENT | PAGE_FLAG_READ_WRITE);
            }
        }
    }
}

