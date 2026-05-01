#include "../include/avltree.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

/*
 * If node doesn't exist it has height of 0
 * Node's height is defined by the maximum between
 * its left and right childrens heights
 */
static int height(avlnode_t *node)
{
    if (!node) return 0;
    int lh, rh;

    if (!node->left) lh = 0;
    else lh = node->left->height;
    
    if (!node->right) rh = 0;
    else rh = node->right->height;

    return 1 + max(rh, lh);
}

/*  Notes: 
 *      n   = function arguemnt node
 *      nr  = nodes right child
 *      nrl = nodes right childs left child
 *      nrr = nodes right childs right child
 *      
 *      Before rotation (Suppose right side is heavy):
 *             n
 *            / \
 *           nl nr
 *             /  \
 *            nrl nrr 
 *      
 *      After rotation:
 *             nr
 *            / \
 *           n  nrr
 *          / \
 *         nl nrl
 *
 *      It's pretty obvious to see that
 *      only nr and n heights changed
 *      
 *      We return the new node that replaces
 *      the where the imbalance occured
 */
static avlnode_t *rotate_left(avlnode_t *node)
{
    avlnode_t *nr = node->right;
    node->right = nr->left;
    nr->left = node;

    node->height = height(node);
    nr->height = height(nr);
    return nr;
}


/*  Notes: 
 *      n = function argument node
 *      nr = nodes right child
 *      nl = nodes left child
 *      nll = nodes left childs left child
 *      nlr = nodes left childs right child
 *      
 *      Before rotation (Suppose left side is heavy):
 *              n
 *             / \
 *            nl nr
 *           / \
 *         nll nlr
 *
 *      After rotation:
 *              nl
 *             /  \
 *           nll   n
 *                / \
 *              nlr nr
 *      
 *      As a function it essentially does the same thing as
 *      function rotate_left
 */
static avlnode_t *rotate_right(avlnode_t *node)
{
    avlnode_t *nl = node->left;
    node->left = nl->right;
    nl->right = node;

    node->height = height(node);
    nl->height = height(nl);
    return nl;
}

static int balance_factor(avlnode_t *node)
{
    int lh, rh;
    if (node == NULL)
        return 0;
    
    if (node->left == NULL)
        lh = 0;
    else
        lh = 1 + node->left->height;
    
    if (node->right == NULL)
        rh = 0;
    else
        rh = 1 + node->right->height;

    return lh - rh;
}

avlnode_t *avltree_insert(avlnode_t *at, avlnode_t *node, avltree_cmp_fn_t cmp_fn)
{
    if (node == NULL || cmp_fn == NULL)
        return at;
    if (at == NULL)
        return node;

    if (cmp_fn(node, at) < 0)
        at->left = avltree_insert(at->left, node, cmp_fn);
    else
        at->right = avltree_insert(at->right, node, cmp_fn);


    int bf = balance_factor(at);

    if (bf > 1)
    {
        if (cmp_fn(node, at->left) < 0)
            return rotate_right(at);
        else
        {
            at->left = rotate_left(at->left);
            return rotate_right(at);
        }
    }

    if (bf < -1)
    {
        if (cmp_fn(node, at->right) > 0)
            return rotate_left(at);
        else
        {
            at->right = rotate_right(at->right);
            return rotate_left(at);
        }
    }

    at->height = height(at);
    
    return at;
}

avlnode_t *avltree_delete(avlnode_t *at, avlnode_t *node)
{
    avlnode_t *temp = NULL;
    if (at == NULL)
        return NULL;
    if (node == NULL)
        return NULL;
    
    if (at->right != NULL)
    {
        temp = at->right;
        while (temp->left != NULL)
            temp = temp->left;

        *at = *node;
        at->right = avltree_delete(at->right, temp);
        if (balance_factor(at) == 2)
        {
            if (balance_factor(at->left) >= 0)
            {
                at = rotate_right(at);
            } else {
                at->left = rotate_left(at->left);
                at = rotate_right(at);
            }
        }
    } else
    {
        return at->left;
    }

    at->height = height(at);
    return at;
}


