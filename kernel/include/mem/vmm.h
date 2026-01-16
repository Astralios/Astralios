#pragma once

#include <stddef.h>
#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#include "arch/x86_64/mem/paging.h"
#endif

typedef enum vmm_flag_t {
    VMM_WRITEABLE           = 1 << 0,
    VMM_EXECUTABLE          = 1 << 1,
    VMM_USER_ACCESSIBLE     = 1 << 2,
    VMM_UNCACHEABLE         = 1 << 3,
    VMM_FRAMEBUFFER_CACHED  = 1 << 4,
    VMM_ZEROED              = 1 << 5,
    VMM_FLAGS_COUNT         = 1 << 6,
} vmm_flag_t;

typedef enum vmm_region_state_t {
    Free,
    Allocated,
    LazyAllocate,
} vmm_region_state_t;

typedef struct vmm_region_t {
    char *name;
    virt_addr_t addr;
    vmm_region_state_t state;
    int flags;
    size_t len;
    struct vmm_region_t *next, *prev;
} vmm_region_t;

typedef struct vmm_t {
    page_table_t page_table;
    vmm_region_t region;
} vmm_t;

typedef enum vmm_alloc_error {
    OUT_OF_MEMORY,
    ERROR_VMM_COUNT,
} vmm_alloc_error;

virt_addr_t vmm_alloc(char *name, size_t len, int flags);

