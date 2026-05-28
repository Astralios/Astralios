#pragma once

#include <bootstub.h>
#include <stdint.h>

typedef struct page_table_t page_table_t;
typedef struct vmm_t vmm_t;

typedef struct int_ctrl_t 
{
    const char *name;
    void (*uninit)(void);
    void (*send_eoi)(uint8_t irq);
    void (*set_mask)(uint8_t irq);
    void (*clear_mask)(uint8_t irq);
} int_ctrl_t;

typedef struct krnl_ctx_t
{
    bootloader_ctx_t* bootloader_ctx;
    page_table_t *pt;
    const int_ctrl_t* interrupt_controller;
    vmm_t *krnl_vmm;
} krnl_ctx_t;

extern krnl_ctx_t krnl_ctx;


