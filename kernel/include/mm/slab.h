#pragma once

#include "arch/x86_64/def.h"
#include "vendor/list.h"

typedef struct slab_cache_t {
    list_t slabs_full, slabs_partial, slabs_free;
    size_t objsize;
    // Number of objects contained in each slab
    size_t objnum;
} slab_cache_t;

typedef struct slab_t {

} slab_t;

void slab_cache_create();

