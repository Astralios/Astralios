#include "../include/avltree.h"
#include <stdbool.h>

#define MAX(a, b) ((a > b) ? a : b)

static inline long height(avltree_t *node)
{
    if (!node) return -1;
    return node->height;
}

static inline int balance_factor(avltree_t *node)
{
    if (!node) return 0;
    return height(node->left) - height(node->right);
}


/*
 * Note: bf stands for balance factor
 */

/* 
 *      x (bf = 2, left heavy)
 *     / \
 *    y  xr
 *   / \
 *  yl  yr
 *
 *  after rotation:
 *
 *     y
 *    / \
 *   yl x
 *     / \
 *    yr xr
 */
static avltree_t *rot_right(avltree_t *x)
{
    avltree_t *y  = x->left;
    avltree_t *yr = y->right;

    y->right = x;
    y->parent = x->parent;
    
    x->parent = y;
    x->left = yr;
    
    if (yr) yr->parent = x;
    if (y->parent)
    {
        if (y->parent->left == x)
            y->parent->left = y;
        else
            y->parent->right = y; 
    }

    x->height = MAX(height(x->left), height(x->right)) + 1;
    y->height = MAX(height(y->left), height(y->right)) + 1;

    return y;
}

/*
 *      x (bf = -2, right heavy)
 *     / \
 *    xl y
 *      / \
 *     yl yr
 *
 * after rotation:
 *
 *     y
 *    / \
 *   x  yr
 *  / \
 * xl yl
 */
static avltree_t *rot_left(avltree_t *x)
{
    avltree_t *y  = x->right;
    avltree_t *yl = y->left;
    
    y->left = x;
    y->parent = x->parent;
    
    x->parent = y;
    x->right = yl;

    if (yl) yl->parent = x;
    if (y->parent)
    {
        if (y->parent->left == x)
            y->parent->left = y;
        else
            y->parent->right = y;
    }

    x->height = MAX(height(x->left), height(x->right)) + 1;
    y->height = MAX(height(y->left), height(y->right)) + 1;

    return y;
}

avltree_t *balance(avltree_t *node)
{
    int bf = balance_factor(node);

    if (bf > 1)
    {
        if (balance_factor(node->left) >= 0) return rot_right(node);
        else {
            node->left = rot_left(node->left);
            return rot_right(node);
        }
    } else if (bf < -1)
    {
        if (balance_factor(node->right) <= 0) return rot_left(node);
        else {
            node->right = rot_right(node->right);
            return rot_left(node);
        }
    }

    return node;
}

void avltree_insert(avltree_t **root, avltree_t *node, avltree_cmp_fn_t cmp_fn)
{
    avltree_t *curr = *root;
    avltree_t *parent = NULL;
    while (curr)
    {
        parent = curr;
        if (cmp_fn(node, curr) < 0) curr = curr->left;
        else curr = curr->right;
    }

    node->parent = parent;
    node->left = NULL;
    node->right = NULL;
    node->height = 0;
    if (!parent)
    {
        *root = node;
        return;
    }

    if (cmp_fn(node, parent) < 0) parent->left = node;
    else parent->right = node;

    curr = parent;
    while (curr)
    {
        curr->height = MAX(height(curr->left), height(curr->right)) + 1;
        avltree_t *balanced = balance(curr);
        if (!balanced->parent) *root = balanced;
        curr = balanced->parent;
    }
}

avltree_t *avltree_find(avltree_t *root, avltree_t *node, avltree_cmp_fn_t cmp_fn)
{
    avltree_t *curr = root;
    while (curr)
    {
        int v = cmp_fn(node, curr);
        if (v == 0) return curr;
        else if (v < 0) curr = curr->left;
        else curr = curr->right;
    }
    return NULL;
}

avltree_t* avltree_delete(avltree_t **root, avltree_t *node, avltree_cmp_fn_t cmp_fn)
{
    avltree_t *to_delete = avltree_find(*root, node, cmp_fn);
    if (!to_delete) return NULL;

    avltree_t *parent = to_delete->parent;

    // If it has no parent then it must be the root
    if (!parent)
    {
        *root = NULL;
        return to_delete;
    }

    bool isLeft = parent->left == to_delete;

    // Case 1: It has no children
    if (!to_delete->right && !to_delete->left)
    {
        if (isLeft) parent->left = NULL;
        else parent->right = NULL;
    } 
    // Case 2: It only has a left child
    else if (!to_delete->right && to_delete->left)
    {
        to_delete->left->parent = parent;
        if (isLeft) parent->left = to_delete->left;
        else parent->right = to_delete->left;
    } 
    // Case 3: It only has a right child
    else if (!to_delete->left && to_delete->right)
    {
        to_delete->right->parent = parent;
        if (isLeft) parent->left = to_delete->right;
        else parent->right = to_delete->right;
    } 
    // Case 4: It has both children
    else {
        avltree_t *curr = to_delete->left;
        
        // Finds the in-order successor
        // BTW it cannot have a right child, because it is the rightmost node
        while (curr->right) curr = curr->right; 
        
        if (curr->left)
        {
            curr->left->parent = curr->parent;
            if (curr->parent) curr->parent->right = curr->left;
        }
        
        if (parent)
        {
            if (isLeft) parent->left = curr;
            else parent->right = curr;
        } else {
            *root = curr;
        }

        curr->left = to_delete->left;
        if (curr->left) curr->left->parent = curr;
        
        curr->right = to_delete->right;
        if (curr->right) curr->right->parent = curr;

        curr->parent = to_delete->parent;
    }

    avltree_t *curr = parent;
    while (curr)
    {
        curr->height = MAX(height(curr->left), height(curr->right)) + 1;
        avltree_t *balanced = balance(curr);
        if (!balanced->parent) *root = balanced;
        curr = balanced->parent;
    }

    return to_delete;
}
