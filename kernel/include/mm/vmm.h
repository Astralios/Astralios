// #pragma once

// #include "misc/helpers.h"
// #include "vendor/list.h"
// #include <stddef.h>
// #ifdef __ARCH_X86_64__
// #include "arch/x86_64/def.h"
// #include "arch/x86_64/mm/paging.h"
// #endif

// #define flag(flag) (1 << flag)

// typedef enum vmm_flag_t {
//     VMM_WRITEABLE,
//     VMM_EXECUTABLE,
//     VMM_USER_ACCESSIBLE,
//     VMM_UNCACHEABLE,
//     VMM_FRAMEBUFFER_CACHED,
//     VMM_ZEROED,
//     VMM_FLAGS_COUNT
// } vmm_flag_t;

// typedef enum vmm_region_state_t {
//     FREE,
//     ALLOCATED, 
//     LAZILY_ALLOCATED,
//     VMM_REGION_STATE_COUNT
// } vmm_region_state_t;

// typedef struct vmm_region_t vmm_region_t;
// typedef struct vmm_region_t {
//     char *name;
//     vaddr_t addr;
//     vmm_region_state_t state;
//     int flags;
//     size_t pages;
//     vmm_region_t *next, *prev;
// } vmm_region_t;

// // Improve this
// typedef struct lazilly_alloc_list_t {
//     list_t list;
//     vmm_region_t *region;
// } lazilly_alloc_list_t;


// typedef enum vmm_allocation_mode_t {
//     NORMAL,
//     LAZY,
// } vmm_allocation_mode_t;

// typedef struct vmm_t {
//     page_table_t* pt;
//     vmm_region_t *regions;
//     char *name;
// } vmm_t;

// typedef enum vmm_alloc_error {
//     OUT_OF_MEMORY,
//     ERROR_VMM_COUNT,
// } vmm_alloc_error;

// typedef enum vmm_debug_mode_t {
//     DEBUG_VMM_REGION_PRINT,
// } vmm_debug_mode_t;

// const char *vmm_region_state_to_str(vmm_region_state_t state);
// const char *vmm_flag_to_str(vmm_flag_t flag);

// void vmm_debug(vmm_debug_mode_t mode, ...);
// vmm_t *vmm_init(page_table_t *pt, vmm_region_t *region_list);
// vaddr_t vmm_alloc(vmm_t *vmm, vmm_region_state_t state, char *name, size_t pages, int flags);
// void vmm_free(vmm_t *vmm, vaddr_t vaddr);

// static inline vaddr_t vmm_alloc_with_bytes(vmm_t *vmm, vmm_region_state_t state, size_t bytes, char *name, int flags)
// {
//     size_t pages = align_up(bytes, PAGE_SIZE);
//     return vmm_alloc(vmm, state, name, pages, flags);
// }

// extern list_t lazy_vaddr_list;
