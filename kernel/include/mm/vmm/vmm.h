#pragma once

#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#include <arch/x86_64/def.h>
#endif

#include <libs/libds/include/avltree.h>
#include <vendor/list.h>

#define flags_clear(in)          ((in) = 0)
#define flag_set(in, flag)      ((in) |= (flag))
#define flag_unset(in, flag)    ((in) &= ~(flag))

typedef enum vmm_flags_t {
    VMM_FLAG_FREE = 1 << 0,
    VMM_FLAG_WRITABLE  = 1 << 1,
} vmm_flags_t;

typedef struct vmm_area_t
{
    list_t       area_list;
    avlnode_t    addr_tree;
    avlnode_t    size_tree;
    
    vaddr_t      start;
    size_t       num_pages;

    vmm_flags_t  vmm_flags;
    page_flags_t page_flags;
} vmm_area_t;

typedef struct vmm_t
{
    size_t          free_num_pages;
    size_t          num_pages;
    vaddr_t         start_addr;

    page_table_t*   pt;
    list_t          area_list_head;
    avlnode_t*      size_tree_root;
    avlnode_t*      addr_tree_root;
} vmm_t;

vmm_t*  vmm_init(page_table_t *pt, vmm_area_t *area);
vaddr_t vmm_palloc(vmm_t *vmm, size_t num_pages, vmm_flags_t flags);
void    vmm_free(vmm_t *vmm, vaddr_t addr);


