#pragma once

#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#endif

#include <stddef.h>
#include <stdint.h>

#include "vendor/bitmap.h"
#include "bootstub.h"

typedef enum pmm_debug_mode_t
{
    DEBUG_PMM_REGIONS,
    DEBUG_PMM_FREE_MEM_SIZE,
    DEBUG_PMM_REGIONS_FULL_SIZE,
    DEBUG_PMM_REGION_BITMAP,
} pmm_debug_mode_t;

// TODO: Perhaps add a first free index
typedef struct pmm_region_t
{
    size_t   pages_available;
    struct   pmm_region_t *next;
    bitmap_t bitmap;
} pmm_region_t;

void        pmm_debug(pmm_debug_mode_t mode, ...);
void        pmm_init(void);
phys_addr_t pmm_alloc(size_t pages_count);
void        pmm_free(phys_addr_t addr, size_t pages_count);

