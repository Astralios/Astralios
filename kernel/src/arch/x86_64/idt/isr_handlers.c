#include "arch/x86_64/idt/isr_handlers.h"

static inline void print_eframe(exception_frame_t *eframe)
{
    srprintf("Exception code: %x\n", eframe->exception_code);
    srprintf("Error code: %x\n", eframe->error_code);
    srprintf(
        "r15: %x, r14: %x, r13: %x, r12: %x, r11: %x, r10: %x, r9: %x, r8: %x\nrbp: %x, rdi: %x, rsi: %x, rdx: %x, rcx: %x, rbx: %x, rax: %x\nrip: %x, cs: %x, rflags: %x, rsp: %x, ss: %x\n",
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
        eframe->rip,
        eframe->cs,
        eframe->rflags,
        eframe->rsp,
        eframe->ss);
}

void isr_exception_handler(exception_frame_t *eframe)
{
    print_eframe(eframe);

    switch (eframe->exception_code)
    {
    case 0x00:
        srprintf("Division by 0!");
        break;

    default:
        break;
    }

    for (;;)
        ;
}
