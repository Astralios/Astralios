#include "arch/x86_64/interrupts/isr_handlers.h"
#include "arch/x86_64/cpu.h"
#include "arch/x86_64/def.h"
#include "arch/x86_64/interrupts/controller/pic.h"
#include "arch/x86_64/mm/paging.h"
#include "drivers/ps2/kbd/ps2_kbd.h"
#include "devices/serial.h"
#include "misc/debug.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "tasks/sched.h"
#include "vendor/list.h"
#include "drivers/ps2/ps2_mouse.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef enum exception_t {
    EXCEPTION_DIVISION_ERROR,
    EXCEPTION_DEBUG,
    EXCEPTION_NON_MASKABLE_INTERRUPT,
    EXCEPTION_BREAKPOINT,
    EXCEPTION_OVERFLOW,
    EXCEPTION_BOUND_RANGE_EXCEEDED
} exception_t;

static char *exception_code_to_str[] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    [16] = "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    [28] = "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    NULL,
    NULL,
};

static char *page_fault_err_code_to_str[] = {
    "Present",
    "Write",
    "User",
    "Reserved Write",
    "Instruction Fetch",
    "Protection Key",
    "Shadow Stack",
    [15] = "Software Guard Extension",
};

static inline void print_eframe(exception_frame_t *eframe)
{ 
    srprintf("\n");
    srprintf("----- %s -----\n", exception_code_to_str[eframe->exception_code]);
    srprintf(
        "r15: %x, r14: %x, r13: %x\nr12: %x, r11: %x, r10: %x\nr9:  %x, r8:  %x, rbp: %x\nrdi: %x, rsi: %x, rdx: %x\nrcx: %x, rbx: %x, rax: %x\nrcs: %x, cs:  %x, rflags: %x\nrsp: %x, rip:  %x\n\n",
        eframe->r15,
        eframe->r14,
        eframe->r13,
        eframe->r12,
        eframe->r11,
        eframe->r10,
        eframe->r9,
        eframe->r8,
        eframe->rbp,
        eframe->rdi,
        eframe->rsi,
        eframe->rdx,
        eframe->rcx,
        eframe->rbx,
        eframe->rax,
        eframe->ss,
        eframe->cs,
        eframe->rflags,
        eframe->rsp,
        eframe->rip);
}

// TODO: Could be made better
static inline void print_flags(uint64_t flags, uint8_t num_bits, char **str_table)
{
    for (int i = 0; i < num_bits; i++)
    {
        bool bit = (flags >> i) & 0b1;
        bool next_bit = (flags >> (i + 1)) & 0b1;
        if (bit) srprintf("%s", str_table[i]);
        if (next_bit && (i != 0 || bit)) srprintf(" | ");
    }
    srprintf("\n");
}

// TODO: Maybe somehow get the symbols and print exactly where it faulted
void isr_exception_handler(exception_frame_t *eframe)
{

    switch (eframe->exception_code)
    {
    case 0xE:
    {
        // list_t *curr = lazy_vaddr_list.next;
        // if (!list_empty(curr))
        // {
        //     vaddr_t fault_addr = read_cr2();
        //     while (curr)
        //     {
        //         lazilly_alloc_list_t *node = ((lazilly_alloc_list_t*)curr);
        //         if (node->region->addr == fault_addr)
        //         {
        //             for (size_t i = 0; i < node->region->pages; i++)
        //             {
        //                 map_to_pt((page_table_t*)read_cr3(), pmm_alloc(1), node->region->addr + i * PAGE_SIZE, PAGE_P | PAGE_RW);
        //             }
        //             return;
        //         }
        //         curr = curr->next;
        //     }
        // }
        
        srprintf("\e[0;31m");
        print_eframe(eframe);
    
        uint64_t err = eframe->error_code;
        uint8_t num_bits = 32;

        srprintf("----- Error Code -----\n");
        print_flags(err, num_bits, page_fault_err_code_to_str);
        break; 
    }
    default:
        break;
    }

    for (;;)
        ;
}

typedef enum interrupt_t {
    PIT_INT = 0x20,
    KBD_INT,
    MOUSE_INT = 0x2C,
} interrupt_t;

void isr_interrupt_handler(interrupt_frame_t *iframe)
{
    switch (iframe->vector_number) {
    case PIT_INT:
    {
        scheduler_switch();
        break;
    }
    case KBD_INT:
        ps2_keyboard_callback(); 
        break;
    case MOUSE_INT: 
        ps2_mouse_callback();
        break;
    }

    pic_send_eoi(iframe->vector_number);
}
