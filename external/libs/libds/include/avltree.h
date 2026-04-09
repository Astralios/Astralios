#pragma once

#include <stddef.h>

typedef struct avltree_t avltree_t;
typedef struct avltree_t {
    avltree_t   *left, *right, *parent;
    long        height;
} avltree_t;

typedef int (avltree_cmp_fn_t)(avltree_t *a, avltree_t *b);

void        avltree_insert(avltree_t **root, avltree_t *node, avltree_cmp_fn_t cmp_fn);
avltree_t* avltree_delete(avltree_t **root, avltree_t *node, avltree_cmp_fn_t cmp_fn);
avltree_t* avltree_find(avltree_t *root, avltree_t *node, avltree_cmp_fn_t cmp_fn);

