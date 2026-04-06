#include "mm/vmm/vmm.h"
#include "misc/helpers.h"
#include "misc/todo.h"

#define dummy_avl_node(size) ((vmm_area_t){ .num_pages = size })

int size_tree_cmp_fn(avltree_t *a, avltree_t *b)
{
    vmm_area_t *a_area = container_of(a, vmm_area_t, size_tree);
    vmm_area_t *b_area = container_of(b, vmm_area_t, size_tree);

    if (a_area->num_pages >= b_area->num_pages) return 0;
    if (a_area->num_pages < b_area->num_pages)  return -1;
}

vaddr_t vmm_palloc(vmm_t *vmm, size_t num_pages, vmm_flag_t flags)
{
    if (num_pages == 0) return 0;
    todo(vmm_palloc);
    // avltree_t *node = avltree_find(&vmm->root, &dummy_avl_node(num_pages).size_tree, size_tree_cmp_fn);
    // avltree_remove(&vmm->root node);
}