#include <kernel.h>
#include <mm/vheap.h>
#include <vendor/list.h>
#include <mm/slab.h>

// TODO: Maybe tweak with alignment
// TODO: This will corrupt memory if obj_size < sizeof(uintptr_t)
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



