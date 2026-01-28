#include <stdint.h>
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

    debug("Test");
    
    int *a = paddr_ptr(pmm_alloc(1));
    *a = 5;
    *a = 6;

    hcf();
}
