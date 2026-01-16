#pragma once

#include <stdint.h>

typedef struct tss_t
{
    uint32_t reserved0;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint64_t reserved1;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved2;
    uint16_t reserved3;
    uint16_t iopb;
} __attribute__((packed)) tss_t;

typedef struct tss_entry_t {
    uint16_t low_limit;
    uint16_t low_base;
    uint8_t  mid_base;
    uint8_t  access_byte;
    uint8_t  limit_and_flags;
    uint8_t  high_base;
    uint32_t highest_base;
    uint32_t reserved;
} __attribute__((packed)) tss_entry_t;

extern tss_t tss;

void tss_init(void);