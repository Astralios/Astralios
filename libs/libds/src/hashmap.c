#include "hashmap.h"

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MALLOC kmalloc
#define FREE   kfree

#define TOMBSTONE       (void*)(-1)
#define DEFAULT_HM_CAP  16

static uint64_t fnv_hash(char *s, int len) {
    uint64_t hash = 0xcbf29ce484222325;
    for (int i = 0; i < len; i++) {
        hash *= 0x100000001b3;
        hash ^= (unsigned char)s[i];
    }
    return hash;
}

static inline bool hashmap_should_resize(hashmap_t *hm)
{
    int pr = 100 * hm->len / hm->cap;
    return pr >= 75;
}

static bool hashmap_resize(hashmap_t *hm)
{
    hashmap_entry_t *new_entries = MALLOC(2 * hm->cap * sizeof(hashmap_entry_t));
    if (!new_entries)
        return false;

    hashmap_entry_t *old_entries = hm->entries;
    size_t old_cap = hm->cap;
    hm->entries = new_entries;
    hm->cap *= 2;
    for (size_t i = 0; i < hm->cap; ++i)
    {
        hm->entries[i].keysz = 0;
        hm->entries[i].key = TOMBSTONE;
        hm->entries[i].val = NULL;
    }
 
    for (size_t i = 0; i < old_cap; ++i)
    {
        if (old_entries[i].key != TOMBSTONE)
        {
            uint64_t hash = fnv_hash((char*)old_entries[i].key, old_entries[i].keysz);
            size_t idx = hash % hm->cap;
            
            hm->entries[idx].keysz = old_entries[i].keysz;
            hm->entries[idx].key = old_entries[i].key;
            hm->entries[idx].val = old_entries[i].val;
        }
    }
    
    FREE(old_entries);
    return true;
}

void hashmap_init(hashmap_t *hm)
{   
    hm->cap = DEFAULT_HM_CAP;
    hm->len = 0;
    hm->entries = MALLOC(hm->cap * sizeof(hashmap_entry_t));
    for (size_t i = 0; i < hm->cap; ++i)
    {
        hm->entries[i].keysz = 0;
        hm->entries[i].key = TOMBSTONE;
        hm->entries[i].val = NULL;
    }
} 

bool hashmap_put(hashmap_t *hm, void *key, size_t keysz, void *val)
{
    if (hashmap_should_resize(hm))
        if (!hashmap_resize(hm))
            return false;

    uint64_t hash = fnv_hash((char*)key, keysz);
    size_t idx = hash % hm->cap;
    printfln("Put: %ld", idx);

    for (size_t i = 0; i < hm->cap; ++i)
    {   
        hashmap_entry_t *entry = &hm->entries[(idx + i) % hm->cap];
        
        if (entry->key == TOMBSTONE)
        {
            entry->keysz = keysz;
            entry->key = key;
            entry->val = val;
            hm->len++;
            return true;
        }
    }
    return false;
}

bool hashmap_get(hashmap_t *hm, void *key, size_t keysz, void **val)
{
    uint64_t hash = fnv_hash((char*)key, keysz);
    size_t idx = hash % hm->cap;
    printfln("Get: %ld", idx);
    for (size_t i = 0; i < hm->cap; ++i)
    {   
        hashmap_entry_t *entry = &hm->entries[(idx + i) % hm->cap];
        
        if (entry->key != TOMBSTONE && entry->keysz == keysz && memcmp(entry->key, key, keysz) == 0)
        {   
            *val = entry->val;
            return true;
        }
    }

    printfln("!");

    return false;   
}

bool hashmap_remove(hashmap_t *hm, void *key, size_t keysz)
{   
    if (hm->len == 0)
        return false;

    uint64_t hash = fnv_hash((char*)key, keysz);
    size_t idx = hash % hm->cap;

    for (size_t i = 0; i < hm->cap; ++i)
    {   
        hashmap_entry_t *entry = &hm->entries[(idx + i) % hm->cap];
        if (entry->key != TOMBSTONE && entry->keysz == keysz && memcmp(entry->key, key, keysz) == 0)
        {   
            entry->key = TOMBSTONE;
            entry->val = NULL;
            hm->len--;
            return true;
        }
    }

    return false;   
}
