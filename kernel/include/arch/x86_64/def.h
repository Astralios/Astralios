#pragma once

#include <stdint.h>
#include "bootstub.h"

#define PAGE_SIZE 4096
#define TO_PAGES(bytes) (bytes / PAGE_SIZE)

extern kernel_params_t *kernel_params;

typedef uint64_t vaddr_t;
typedef uint64_t paddr_t;

/// @brief Casts any pointer to a virt_addr_t
#define cast_vaddr(ptr) ((vaddr_t)ptr)
/// @brief Turns a physical address to a virtual address
#define to_vaddr(paddr) cast_vaddr((((char*)(paddr)) + kernel_params->hhdm))
/// @brief Turns a virtual address to a physical address
#define to_paddr(vaddr) ((paddr_t)(((char*)vaddr) - kernel_params->hhdm))
#define paddr_ptr(paddr) ((void*)to_vaddr(paddr))
