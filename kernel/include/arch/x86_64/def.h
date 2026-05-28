#pragma once

#include "kernel.h"
#include <stdint.h>

#define PAGE_SIZE 4096

typedef uint64_t vaddr_t;
typedef uint64_t paddr_t;

#define cast_vaddr(ptr) ((vaddr_t)ptr)

#define to_vaddr(paddr) cast_vaddr((((char*)(paddr)) + krnl_ctx.bootloader_ctx->hhdm))
#define to_paddr(vaddr) ((paddr_t)(((char*)(vaddr))  - krnl_ctx.bootloader_ctx->hhdm))

#define paddr_ptr(paddr) ((void*)to_vaddr(paddr))
