#pragma once
#include <stddef.h>
#include <stdbool.h>
typedef struct
{
    void *key;
    void *val; 
    size_t keysz;
} hashmap_entry_t;

typedef struct
{
    hashmap_entry_t *entries;
    size_t cap, len;
} hashmap_t;

void hashmap_init(hashmap_t *hm);
bool hashmap_put(hashmap_t *hm, void *key, size_t keysz, void *val);
bool hashmap_get(hashmap_t *hm, void *key, size_t keysz, void **val);
bool hashmap_remove(hashmap_t *hm, void *key, size_t keysz);
// TOOD: Add hashmap free
