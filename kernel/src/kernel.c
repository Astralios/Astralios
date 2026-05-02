#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>

#include <bootstub.h>

#include <misc/debug.h>

#include <mm/pmm/pmm.h>
#include <mm/vmm/vmm.h>

#include <devs/serial.h>

#include <drvs/ps2/ps2.h>
#include <drvs/ps2/kbd/ps2_kbd.h>
#include <drvs/ps2/ps2_mouse.h>

#include <firmware/acpi/acpi.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/def.h>
#include <arch/x86_64/mm/paging.h>
#include <arch/x86_64/cpu/cpu.h>
#include <arch/x86_64/desc/gdt.h>
#include <arch/x86_64/desc/idt.h>
#include <arch/x86_64/ints/pic.h>
#include <arch/x86_64/ints/apic.h>
#endif

kernel_context_t *kernel_context = NULL;
page_table_t *root_pt = NULL;
extern vaddr_t hhdm_end;

void kpaging_init(page_table_t **root_pt)
{
    uint64_t root_pt_paddr = pmm_palloc(1);
    *root_pt = paddr_ptr(root_pt_paddr);
    memset((*root_pt)->entries, 0, PAGE_SIZE);
    map_kernel_to_pt(*root_pt);
    map_memmap_to_pt(*root_pt);
    write_cr3(root_pt_paddr);
    info(paging_init, "Initalized!");
}

void kmain(kernel_context_t *context)
{
    kernel_context = context;
    serial_init();
    gdt_init();
    idt_init();
    tss_init();    
    pmm_init();
    kpaging_init(&root_pt);

    acpi_init();

    pic_init();
    apic_init();

    ps2_init();
    ps2_keyboard_init();
    ps2_mouse_init();

    vmm_t vmm;
    vmm_init(&vmm, root_pt, hhdm_end, (32 * 1024 * 1024 * 1024l) / PAGE_SIZE);
    int *x = vmm_palloc(&vmm, 1, PAGE_FLAG_READ_WRITE);
    *x = 5;
    

    hcf();
}
