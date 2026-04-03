#pragma once

#include <stddef.h>

typedef struct hashmap_bucket_t hashmap_bucket_t;
typedef struct hashmap_bucket_t {
    size_t hash;
    void *key;
    void *val;
    hashmap_bucket_t *next;
} hashmap_bucket_t;

typedef struct hashmap_t {
    size_t cap, len;

} hashmap_t;

