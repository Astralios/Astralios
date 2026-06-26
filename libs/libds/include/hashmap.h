#pragma once

#include <string.h>
#include <stddef.h>

typedef void*   (*alloc_fn_t)(size_t size);
typedef void    (*free_fn_t)(void *ptr);
typedef size_t  (*hash_fn_t)(void *val);

typedef struct hashmap_entry_t hashmap_entry_t;
typedef struct hashmap_entry_t
{
    size_t hash;
    void *key, *val;
    hashmap_entry_t *next;
} hashmap_entry_t;

typedef enum hashmap_set_mode_t
{
    TAKE,
    COPY,
} hashmap_set_mode_t;

typedef struct hashmap_t
{
    size_t              len, cap;
    size_t              ksize, vsize;
    hashmap_entry_t**   entries;
    alloc_fn_t          alloc_fn;
    free_fn_t           free_fn;
    hash_fn_t           hash_fn;
} hashmap_t;


