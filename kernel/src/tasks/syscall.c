#include "misc/debug.h"
#include <tasks/syscall.h>

#define ARG0(frame) (frame).rax
#define ARG1(frame) (frame).rdi
#define ARG2(frame) (frame).rsi
#define ARG3(frame) (frame).rdx
#define ARG4(frame) (frame).rax

void syscall_handler(interrupt_frame_t *iframe)
{
    uint64_t syscall_number = iframe->rax;   
    if (syscall_number == 1)
    {
        debug("Hello World from userspace!");
    }
}

