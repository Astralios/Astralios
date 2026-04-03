#pragma once

#include <stdint.h>
#include <sys/types.h>

typedef enum rflags_t {
    CARRY_FLAG,
    PARITY_FLAG = 2,
    AUXILIARY_CARRY_FLAG = 4,
    ZERO_FLAGG = 6,
    SIGN_FLAG,
    TRAP_FLAG,
    INTERRUPT_ENABLE_FLAG,
    DIRECTION_FLAG,
    OVERFLOW_FLAG,
    // TODO
    // IO_PRIVILEGE_LEVEL,
    NESTED_TASKS = 14,
    RESUME_FLAG = 16,
    VIRTUAL_8086_MODE,
    ACCESS_CONTROL,
    VIRTUAL_INTERRUPT_FLAG,
    VIRTUAL_INTERRUPT_PENDING,
    ID_FLAG,
} rflags_t;

static inline void sti(void)
{
    __asm__ volatile("sti");
}

static inline void cli(void)
{
    __asm__ volatile("cli");
}

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

static inline uint64_t read_cr2(void)
{
    uint64_t val;
    __asm__ volatile(
        "mov %%cr2, %0" : "=r"(val));
    return val;
}

static inline uint64_t read_eflags(void)
{
     uint64_t val;
    __asm__ volatile(
        "mov %%eflags, %0" : "=r"(val));
    return val;   
}

static inline void write_cr3(uint64_t val)
{
    __asm__ volatile ("mov %0, %%cr3" : : "r" (val) : "memory");
}

static inline void invplg(uint64_t vaddr) {
   asm volatile("invlpg (%0)" ::"r" (vaddr) : "memory");
}

