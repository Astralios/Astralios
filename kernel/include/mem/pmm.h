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
    DEBUG_PMM_ERROR,
    DEBUG_PMM_COUNT,
} pmm_debug_mode_t;

typedef struct pmm_region_t
{
    paddr_t  free_paddr;
    size_t   pages_available;
    size_t   first_free_idx;
    struct   pmm_region_t *next;
    bitmap_t bitmap;
} pmm_region_t;

void        pmm_debug(pmm_debug_mode_t mode, ...);
void        pmm_init(void);
paddr_t     pmm_alloc(size_t pages_count);
void        pmm_free(paddr_t addr, size_t pages_count);
paddr_t     pmm_alloc_with_bytes(size_t bytes);
void        pmm_free_with_bytes(paddr_t addr, size_t bytes);

static inline vaddr_t pmm_valloc(size_t pages_count)
{
    return to_vaddr(pmm_alloc(pages_count));
}

static inline vaddr_t pmm_valloc_with_bytes(size_t pages_count)
{
    return to_vaddr(pmm_alloc_with_bytes(pages_count));
}
