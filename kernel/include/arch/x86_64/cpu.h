#pragma once

#include <stdint.h>

static inline void hcf(void)
{
    for (;;)
    {
        __asm__("hlt");
    }
}

static inline uint64_t read_cr3(void)
{
    uint64_t val;
    __asm__ volatile(
        "mov %%cr3, %0" : "=r"(val));
    return val;
}

static inline void write_cr3(uint64_t val)
{
    __asm__ volatile ("mov %0, %%cr3" : : "r" (val) : "memory");
}

static inline void invplg(uint64_t vaddr) {
   asm volatile("invlpg (%0)" ::"r" (vaddr) : "memory");
}
