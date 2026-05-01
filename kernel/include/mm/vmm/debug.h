#pragma once

#include <mm/vmm/vmm.h>

typedef enum vmm_debug_mode_t
{
    VMM_DEBUG_MODE_PRINT_SIZE_TREE,
    VMM_DEBUG_MODE_PRINT_ADDR_TREE,
} vmm_debug_mode_t;

void vmm_debug(vmm_debug_mode_t mode, ...);

