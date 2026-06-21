#pragma once

typedef unsigned char uchar;

#define RB_NODE_BLACK 0
#define RB_NODE_RED   1

typedef struct rb_node_t rb_node_t;
typedef struct rb_node_t {
    uchar      color;
    void      *value;
    rb_node_t *left, *right, *parent;
} rb_node_t;

typedef struct rb_tree_t {
    rb_node_t *root;
    // If a > b:  1
    // If a < b: -1
    // If a = b:  0
    int (*cmp_fn)(rb_node_t *a, rb_node_t *b);
} rb_tree_t;

#define RB_NODE_INIT(n, k, v) ({(n)->key = (k); (n)->value = (v); (n)->parent = (n)->left = (n)->right = NULL; (n)->color = RB_NODE_BLACK; })
#define RB_TREE_INIT(t) ({ (t)->root = NULL; (t)->cmp_fn = NULL; })



