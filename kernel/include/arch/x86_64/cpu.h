#pragma once

#include "arch/x86_64/def.h"

static inline void hcf(void)
{
    for (;;)
    {
        __asm__("hlt");
    }
}

static inline phys_addr_t read_cr3(void)
{
    phys_addr_t val;
    __asm__ volatile(
        "mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline void write_cr3(phys_addr_t val)
{
    __asm__ volatile ("mov %%cr3, %0" : "=r"(val));
}

static inline void invplg(phys_addr_t addr) {
   asm volatile("invlpg (%0)" ::"r" (addr) : "memory");
}
