#include "rbtree.h"

static inline void replace_parents_child(
        rb_tree_t *tree,
        rb_node_t *parent, 
        rb_node_t *old_child, 
        rb_node_t *new_child
) {
    if (!parent)
    {
        tree->root = new_child;
    } else if (parent->left == old_child)
    {
        parent->left = new_child;
    } else if (parent->right == old_child)
    {
        parent->right = new_child;
    } else {
        return;
    }
    
    if (new_child)
    {
        new_child->parent = parent;
    }
}

static inline void rotate_right(
    rb_tree_t *tree,
    rb_node_t *node
) {
    rb_node_t *parent = node->parent;
    rb_node_t *left_child = node->left;

    node->left = left_child->left;
    if (left_child->right)
    {
        left_child->right->parent = node;
    }

    left_child->right = node;
    node->parent = left_child;
    replace_parents_child(tree, parent, node, left_child);
}

static inline void rotate_left(
    rb_tree_t *tree,
    rb_node_t *node
) {
    rb_node_t *parent = node->parent;
    rb_node_t *right_child = node->right;

    node->right = right_child->left;
    if (right_child->left)
    {
        right_child->left->parent = node;
    }

    right_child->left = node;
    node->parent = right_child;
    replace_parents_child(tree, parent, node, right_child);
}



void rb_tree_insert(rb_tree_t *tree, rb_node_t *node)
{
    (void)tree;
    (void)node;
}
