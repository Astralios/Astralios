#pragma once

#include <kernel.h>
#include <stdint.h>
#include <vendor/list.h>

typedef struct cache_t 
{
    list_t slabs_full; 
    list_t slabs_partial;
    list_t slabs_free;

    size_t obj_size;
    size_t obj_per_slab;
} cache_t;

typedef struct slab_t 
{
    cache_t*        cache;
    list_t          slabs;
    vaddr_t         mem;
    size_t          free_idx;
} slab_t;


