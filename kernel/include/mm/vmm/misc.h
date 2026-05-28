#include "arch/x86_64/def.h"
#include <mm/vmm/vmm.h>

// static inline vmm_area_t* vmm_size_tree_area(avlnode_t *node)
// {
//     return container_of(node, vmm_area_t, size_tree);
// }

// static inline vmm_area_t* vmm_addr_tree_area(avlnode_t *node)
// {
//     return container_of(node, vmm_area_t, addr_tree);
// }

// static inline vmm_area_t* vmm_area_list_area(list_t *list)
// {
//     return container_of(list, vmm_area_t, area_list);
// }

static inline vaddr_t vmm_alloc(vmm_t *vmm, size_t size, page_flags_t page_flags)
{
    size_t num_pages = align_up(size, PAGE_SIZE) / PAGE_SIZE;
    return vmm_palloc(vmm, num_pages, page_flags);
}

