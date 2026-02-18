#include "misc/helpers.h"
#include <stdint.h>
#include <time.h>
#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/idt/idt.h"
#include "arch/x86_64/tss.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/mem/paging.h"
#endif

#include "bootstub.h"
#include "devices/serial.h"
#include "misc/debug.h"
#include "string.h"
#include "mem/pmm.h"
#include "mem/vmm.h"

kernel_params_t *kernel_params = NULL;

void kmain(kernel_params_t *params)
{
    kernel_params = params;
    serial_init();
    gdt_init();
    idt_init();
    tss_init();
    pmm_init();    

    uint64_t root_pt_paddr = pmm_alloc(1);
    
    srdebug(kmain, "Root page table addr: %x", root_pt_paddr);
    
    page_table_t *root_pt = paddr_ptr(root_pt_paddr); 
    memset(root_pt->entries, 0, PAGE_SIZE);
    map_kernel_to_pt(root_pt);
    map_memmap_to_pt(root_pt); 
    write_cr3(root_pt_paddr);

    srdebug(kmain, "Set up the new page table");

    vmm_region_t *root_vmm_regions = NULL;
    vaddr_t hhdm_end = 0;
    for (size_t i = 0; i < kernel_params->memmap.entry_count; i++)
    {
        memmap_entry_t entry = kernel_params->memmap.entries[i];
        if (entry.type == MEMMAP_USABLE || entry.type == MEMMAP_FRAMEBUFFER || entry.type == MEMMAP_EXECUTABLE_AND_MODULES || entry.type == MEMMAP_BOOTLOADER_RECLAIMABLE)
        {
            char *name;
            switch (entry.type) {
            case MEMMAP_USABLE:
                name = "Usable";
                break;
            case MEMMAP_FRAMEBUFFER:
                name = "Framebuffer";
                break;
            case MEMMAP_EXECUTABLE_AND_MODULES:
                name = "Kernel";
                break;
            case MEMMAP_BOOTLOADER_RECLAIMABLE:
                name = "Bootloader Reclaimable";
                break;
            }
            
            vaddr_t end = to_vaddr(entry.base + entry.length);
            if (entry.type != MEMMAP_EXECUTABLE_AND_MODULES && end > hhdm_end) hhdm_end = end;

            vmm_region_t *region = paddr_ptr(pmm_alloc(BYTES_TO_FULL_PAGES(sizeof(vmm_region_t))));
            region->addr = to_vaddr(entry.base);
            region->pages = ALIGN_UP(entry.length, PAGE_SIZE) / PAGE_SIZE;
            region->name = name;
            region->state = ALLOCATED;
            if (root_vmm_regions) root_vmm_regions->prev = region;
            region->next = root_vmm_regions;
            root_vmm_regions = region;
        }
    }

    vmm_debug(DEBUG_VMM_REGION_PRINT, root_vmm_regions);

    srdebug(kmain, "kernel vaddr: %x; kernel paddr: %x", kernel_params->kernel_addr.virtual_base, kernel_params->kernel_addr.physical_base);
    srdebug(kmain, "hhdm start: %x", kernel_params->hhdm);
    srdebug(kmain, "hhdm_end: %x", hhdm_end);
    vmm_region_t *heap_region = paddr_ptr(pmm_alloc(BYTES_TO_FULL_PAGES(sizeof(vmm_region_t))));
    heap_region->name = "Heap";
    heap_region->addr = hhdm_end;
    heap_region->pages = (2 * 1024 * 1024 * 1024l) / PAGE_SIZE;
    heap_region->state = FREE;
    heap_region->prev = NULL;
    heap_region->next = NULL;

    vmm_t vmm_heap = { .regions = heap_region, .pt = root_pt };
    vmm_debug(DEBUG_VMM_REGION_PRINT, vmm_heap.regions);
    int *x = (void*)vmm_alloc(&vmm_heap, "For my int", 4096 * 2, PAGE_RW | PAGE_P);
    srdebug(kmain, "%x: ", x);
    *((char*)x) = 5;
    vmm_debug(DEBUG_VMM_REGION_PRINT, vmm_heap.regions);
    vmm_free(&vmm_heap, (vaddr_t)x);
    vmm_debug(DEBUG_VMM_REGION_PRINT, vmm_heap.regions);
    hcf();
}
