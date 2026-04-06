#include "arch/x86_64/arch.h"
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/tss.h"
#include "arch/x86_64/interrupts/idt.h"

void arch_init(void)
{
    gdt_init();
    idt_init();
    tss_init();
}