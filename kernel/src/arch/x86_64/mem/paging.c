#include <stddef.h>

#include "arch/x86_64/mem/paging.h"
#include "arch/x86_64/def.h"
#include "arch/x86_64/cpu.h"
#include "string.h"
#include "bootstub.h"
#include "mem/pmm.h"
#include "misc/helpers.h"
#include "arch/x86_64/cpu.h"

extern page_table_t *pml4;
extern kernel_params_t *kernel_params;

static virt_addr_t hhdm_end;

static void pml4_init(void)
{
    phys_addr_t cr3 = read_cr3();
    pml4 = (page_table_t*)to_vaddr((cr3 & ~0xFFF));
}

static void set_page_entry(page_entry_t *entry)
{
    if (!(*entry & PAGE_PRESENT))
    {
        *entry = 0;
        *entry |= (PAGE_PRESENT | PAGE_READ_WRITE | PAGE_USER_SUPERVISOR);
        *entry |= pmm_alloc(PAGE_SIZE / PAGE_SIZE) & PAGE_PHYSICAL_ADDRESS_MASK;
        memset((void*)to_vaddr((*entry & PAGE_PHYSICAL_ADDRESS_MASK)), 0, PAGE_SIZE);
    }
}

void map_page_table(phys_addr_t phys_addr, virt_addr_t virt_addr, uint16_t flags)
{
    size_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    size_t pml3_idx = (virt_addr >> 30) & 0x1FF;
    size_t pml2_idx = (virt_addr >> 21) & 0x1FF;
    size_t pml_idx  = (virt_addr >> 12) & 0x1FF;
 
    set_page_entry(&pml4->entries[pml4_idx]);

    page_table_t *pml3 = (void*)to_vaddr((pml4->entries[pml4_idx] & PAGE_PHYSICAL_ADDRESS_MASK));
    set_page_entry(&pml3->entries[pml3_idx]);

    page_table_t *pml2 =(void*)to_vaddr((pml3->entries[pml3_idx] & PAGE_PHYSICAL_ADDRESS_MASK));
    set_page_entry(&pml2->entries[pml2_idx]);

    page_table_t *pml = (void*)to_vaddr((pml2->entries[pml2_idx] & PAGE_PHYSICAL_ADDRESS_MASK));
    pml->entries[pml_idx] = (phys_addr & PAGE_PHYSICAL_ADDRESS_MASK) | (flags & 0xFFF);
}

void unmap_page_table(virt_addr_t virt_addr)
{
    size_t pml4_idx = (virt_addr >> 39) & 0x1FF;
    size_t pml3_idx = (virt_addr >> 30) & 0x1FF;
    size_t pml2_idx = (virt_addr >> 21) & 0x1FF;
    size_t pml_idx  = (virt_addr >> 12) & 0x1FF;
 
    page_table_t *pml3 = (void*)to_vaddr((pml4->entries[pml4_idx] & PAGE_PHYSICAL_ADDRESS_MASK));
    page_table_t *pml2 = (void*)to_vaddr((pml3->entries[pml3_idx] & PAGE_PHYSICAL_ADDRESS_MASK));
    page_table_t *pml  = (void*)to_vaddr((pml2->entries[pml2_idx] & PAGE_PHYSICAL_ADDRESS_MASK));
    pml->entries[pml_idx] = 0;
}


static void map_section(char section_begin[], char section_end[], uint8_t flags)
{
    uint64_t offset = ALIGN_DOWN((uint64_t)section_end - (uint64_t)section_begin, PAGE_SIZE);
    uint64_t pages = ALIGN_UP(((uint64_t)section_end - (uint64_t)ALIGN_DOWN((uint64_t)section_begin, PAGE_SIZE)), PAGE_SIZE) / PAGE_SIZE;

    for (uint64_t i = 0; i < pages; i++)
    {
        map_page_table(
            kernel_params->kernel_addr.physical_base + offset + i * PAGE_SIZE,
            kernel_params->kernel_addr.virtual_base + offset + i * PAGE_SIZE,
            flags);
    }
}

static void map_kernel(void)
{
    map_section(section_text_begin, section_text_end, PAGE_PRESENT);
    map_section(section_const_data_begin, section_const_data_end, PAGE_PRESENT);
    map_section(section_mut_data_begin, section_mut_data_end, PAGE_PRESENT | PAGE_READ_WRITE);
}

static void map_memmap(void)
{
    for (uint64_t i = 0; i < kernel_params->memmap.entry_count; i++)
    {
        memmap_entry_t entry = kernel_params->memmap.entries[i];
        if (entry.type == MEMMAP_BOOTLOADER_RECLAIMABLE || entry.type == MEMMAP_USABLE || entry.type == MEMMAP_FRAMEBUFFER || entry.type == MEMMAP_EXECUTABLE_AND_MODULES)
        {
            uint64_t base = entry.base;
            uint64_t length = entry.length;
            uint64_t end = (entry.base + length);
            hhdm_end = end + kernel_params->hhdm;

            for (uint64_t current = base; current < end; current += PAGE_SIZE)
            {
                if (entry.type == MEMMAP_FRAMEBUFFER)
                {
                    map_page_table(current, current + kernel_params->hhdm, PAGE_PRESENT | PAGE_READ_WRITE | PAGE_ATTRIBUTE_TABLE | PAGE_WRITE_THROUGH);
                }
                else
                {
                    map_page_table(current, current + kernel_params->hhdm, PAGE_PRESENT | PAGE_READ_WRITE);
                }
            }
        }
    }
}

void paging_init(void)
{
    pml4_init();
    map_kernel();
    map_memmap();
    write_cr3(to_paddr(pml4));
}
