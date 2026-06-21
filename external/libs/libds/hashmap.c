#include "hashmap.h"

void hashmap_init(
        hashmap_t *map, 
        size_t cap, 
        size_t ksize, 
        size_t vsize, 
        alloc_fn_t alloc_fn, 
        free_fn_t free_fn, 
        hash_fn_t hash_fn
)
{
    if (!map) return;

    map->len = 0;
    map->cap = cap;
    map->alloc_fn = alloc_fn;
    map->free_fn = free_fn;
    map->hash_fn = hash_fn;
    map->ksize = ksize;
    map->vsize = vsize;
    map->entries = (hashmap_entry_t**)map->alloc_fn(map->cap * sizeof(hashmap_entry_t*));

    return;
}

hashmap_entry_t* hashmap_entry_create(hashmap_t *map)
{
    hashmap_entry_t *entry = map->alloc_fn(sizeof(hashmap_entry_t));
    *entry = (hashmap_entry_t){0};

    return entry;
}

void hashmap_set(hashmap_t *map, void *key, void *val, hashmap_set_mode_t mode)
{
    size_t hash = map->hash_fn(key);
    size_t idx = hash % map->cap;

    switch (mode) {
    case TAKE: {
        hashmap_entry_t *entry = hashmap_entry_create(map);
        entry->hash = hash;
        entry->key = key;
        entry->val = val;
        
         

        break;
    }
    default:
        break;
    }
}
