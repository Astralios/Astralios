#pragma once

typedef enum pmm_debug_mode_t
{
    PMM_DEBUG_MODE_REGIONS,
    PMM_DEBUG_MODE_FREE_MEM_SIZE,
} pmm_debug_mode_t;

void pmm_debug(pmm_debug_mode_t mode, ...);