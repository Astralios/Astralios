#pragma once

#ifdef __ARCH_X86_64__
#include <arch/x86_64/cpu/cpu.h>
#endif

static inline void kabort(void)
{
    hcf();
}
