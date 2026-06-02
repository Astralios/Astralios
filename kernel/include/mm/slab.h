#pragma once

#include <kernel.h>
#include <stdint.h>
#include <vendor/list.h>

/* TODO: Add colouring */
/* TODO: Add flags */
typedef struct
{
    list_t list;
    list_t slabs_free;
    list_t slabs_partial;
    list_t slabs_full;
    
    size_t obj_size;
    size_t num_objs_per_slab;

    size_t total_num_objs;
    size_t total_num_slabs;
    

    /* It's usually 1 */
    size_t num_pages_per_slab;
    const char *name;
} cache_t;

typedef struct
{
    list_t list;
    
    size_t num_objs_inuse;
    void *smem;
    // TODO: prev pointer is not needed
    list_t freelist;
} slab_t;

cache_t *cache_create(const char *name, size_t obj_size);


