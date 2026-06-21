#pragma once

#include <stdint.h>

#include <arch/x86_64/desc/tss.h>

extern void reload_gdt();

typedef struct gdt_entry_t
{
    uint16_t low_limit;
    uint16_t low_base;
    uint8_t  mid_base;
    uint8_t  access_byte;
    uint8_t  limit_and_flags;
    uint8_t  high_base;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt_t
{
    gdt_entry_t null_descriptor;
    gdt_entry_t kernel_code_segment;
    gdt_entry_t kernel_data_segment;
    gdt_entry_t user_code_segment;
    gdt_entry_t user_data_segment;
    tss_entry_t tss_segment;
} __attribute__((packed)) gdt_t;

typedef struct gdtr_t
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) gdtr_t;

extern gdt_t gdt;
void gdt_init(void);

