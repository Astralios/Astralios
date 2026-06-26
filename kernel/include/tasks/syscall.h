#pragma once

#include "arch/x86_64/ints/isr_handlers.h"

extern void syscall_set_number(uint64_t number);

void syscall_handler(interrupt_frame_t *iframe);


