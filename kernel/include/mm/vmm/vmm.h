#pragma once

#ifdef __ARCH_X86_64__
#include "arch/x86_64/mm/paging.h"
#include "arch/x86_64/def.h"
#endif

#include "vendor/avltree.h"
#include "vendor/list.h"

typedef enum vmm_area_state_t {
    FREE,
    ALLOCATED,
    LAZILY_ALLOCATED,
    VMM_AREA_STATE_COUNT,
} vmm_area_state_t;

typedef enum vmm_flag_t {
    VMM_WRITABLE,
} vmm_flag_t;

typedef struct vmm_area_t vmm_area_t;
typedef struct vmm_area_t
{
    list_t      area_list;
    avltree_t   addr_tree;
    avltree_t   size_tree;
    vaddr_t     addr;
    size_t      num_pages;
    vmm_flag_t  flags;
} vmm_area_t;

typedef struct vmm_t
{
    page_table_t*   pt;
    avltree_t*      root;
} vmm_t;

vaddr_t vmm_palloc(vmm_t *vmm, size_t num_pages, vmm_flag_t flags);
void    vmm_free(vmm_t *vmm, vaddr_t addr);
