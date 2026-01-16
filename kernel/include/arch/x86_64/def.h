#pragma once

#include <stdint.h>
#include "bootstub.h"

#define PAGE_SIZE 4096

extern kernel_params_t *kernel_params;

typedef uint64_t virt_addr_t;
typedef uint64_t phys_addr_t;

/// @brief Casts any pointer to a virt_addr_t
#define cast_vaddr(ptr) ((virt_addr_t)ptr)
/// @brief Turns a physical address to a virtual address
#define to_vaddr(paddr) cast_vaddr(((char*)paddr) + kernel_params->hhdm)
/// @brief Turns a virtual address to a physical address
#define to_paddr(vaddr) ((phys_addr_t)(((char*)vaddr) - kernel_params->hhdm))

