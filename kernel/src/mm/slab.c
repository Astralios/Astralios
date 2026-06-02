#include <kernel.h>
#include <mm/vheap.h>
#include <stdbool.h>
#include <vendor/list.h>
#include <mm/slab.h>

#define SLAB_MEM(cache) (cache->obj_size * cache->num_objs_per_slab + sizeof(slab_t))

// TODO: Maybe tweak with alignment
// TODO: This will corrupt memory if obj_size < 2 * sizeof(uintptr_t)
cache_t *cache_create(const char *name, size_t obj_size)
{
    // TODO: Don't use vmalloc
    cache_t *cache = vmalloc(sizeof(cache_t));
    
    list_init(&cache->list);
    list_init(&cache->slabs_free);
    list_init(&cache->slabs_partial);
    list_init(&cache->slabs_full);
    
    cache->obj_size = obj_size;
    // TODO: Change this to a better method
    cache->num_pages_per_slab = 1;
    cache->num_objs_per_slab = cache->num_pages_per_slab * PAGE_SIZE / cache->obj_size;
    cache->total_num_objs = 0;
    cache->total_num_slabs = 0;
    cache->name = name;

    return cache;
}

bool cache_grow(cache_t *cache)
{
    // TODO: Don't use vmalloc
    slab_t *slab = vmalloc(SLAB_MEM(cache));
    if (!slab)
    {
        return false;
    }

    list_init(&slab->list);
    list_init(&slab->freelist);
    
    slab->num_objs_inuse = 0;
    slab->cache = cache;
    slab->smem = (void*)((vaddr_t)slab + sizeof(slab_t));
    
    for (size_t i = 0; i < cache->num_objs_per_slab; ++i)
    {
        list_t *list = slab->smem + i * cache->obj_size;
        list_append(&slab->freelist, list);
    }

    list_append(&cache->slabs_free, &slab->list);

    return true;
}



