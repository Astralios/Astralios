#include <arch/x86_64/ints/exceptions.h>
#include <devs/serial.h>

static char *exception_code_to_str[] = {
    [EXCEPTION_DIVISION_ERROR] = "Division Error",
    [EXCEPTION_DEBUG] = "Debug",
    [EXCEPTION_NON_MASKABLE_INTERRUPT] = "Non-maskable Interrupt",
    [EXCEPTION_BREAKPOINT] = "Breakpoint",
    [EXCEPTION_OVERFLOW] = "Overflow",
    [EXCEPTION_BOUND_RANGE_EXCEEDED] = "Bound Range Exceeded",
    [EXCEPTION_INVALID_OPCODE] = "Invalid Opcode",
    [EXCEPTION_DEVICE_NOT_AVAILABLE] = "Device Not Available",
    [EXCEPTION_DOUBLE_FAULT] = "Double Fault",
    [EXCEPTION_COPROCESSOR_SEGMENT_OVERRUN] = "Coprocessor Segment Overrun",
    [EXCEPTION_INVALID_TSS] = "Invalid TSS",
    [EXCEPTION_SEGMENT_NOT_PRESENT] = "Segment Not Present",
    [EXCEPTION_STACK_SEGMENT_FAULT] = "Stack-Segment Fault",
    [EXCEPTION_GENERAL_PROTECTION_FAULT] = "General Protection Fault",
    [EXCEPTION_PAGE_FAULT] = "Page Fault",
    [EXCEPTION_X87_FLOATING_POINT_EXCEPTION] = "x87 Floating-Point Exception",
    [EXCEPTION_ALIGNMENT_CHECK] = "Alignment Check",
    [EXCEPTION_MACHINE_CHECK] = "Machine Check",
    [EXCEPTION_SIMD_FLOATING_POINT_EXCEPTION] = "SIMD Floating-Point Exception",
    [EXCEPTION_VIRTUALIZATION_EXCEPTION] = "Virtualization Exception",
    [EXCEPTION_CONTROL_PROTECTION_EXCEPTION] = "Control Protection Exception",
    [EXCEPTION_HYPERVISOR_INJECTION_EXCEPTION] = "Hypervisor Injection Exception",
    [EXCEPTION_VMM_COMMUNICATION_EXCEPTION] = "VMM Communication Exception",
    [EXCEPTION_SECURITY_EXCEPTION] = "Security Exception",
};

void print_eframe(exception_frame_t *eframe)
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


