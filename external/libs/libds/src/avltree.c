#include "../include/avltree.h"

#define max(a, b) ((a) > (b) ? (a) : (b))

static int node_height(avlnode_t *node)
{
    return node ? node->height : 0;
}

static void update_height(avlnode_t *node)
{
    if (!node) return;
    node->height = 1 +max(node_height(node->left), node_height(node->right));
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

    update_height(node);
    update_height(nr);

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

    update_height(node);
    update_height(nl);

    return nl;
}

static int balance_factor(avlnode_t *node)
{
    if (node == NULL) return 0;
    return node_height(node->left) - node_height(node->right);
}

static avlnode_t *rebalance(avlnode_t *node)
{
    if (!node) return NULL;
    update_height(node);
    int bf = balance_factor(node);

    if (bf > 1)
    {
        if (balance_factor(node->left) < 0) node->left = rotate_left(node->left);
        return rotate_right(node);
    }

    if (bf < -1)
    {
        if (balance_factor(node->right) > 0) node->right = rotate_right(node->right);
        return rotate_left(node);
    }

    return node;
}

avlnode_t *avltree_insert(avlnode_t *at, avlnode_t *node, avltree_cmp_fn_t cmp_fn)
{
    if (node == NULL || cmp_fn == NULL) return at;

    if (at == NULL)
    {
        node->left = NULL;
        node->right = NULL;
        node->height = 1;
        return node;
    }

    if (cmp_fn(node, at) < 0)
    {
        at->left = avltree_insert(
                at->left,
                node, 
                cmp_fn);
    } else {
        at->right = avltree_insert(
                at->right, 
                node, 
                cmp_fn);
    }

    return rebalance(at);
}

static avlnode_t *find_min(avlnode_t *node)
{
    while (node->left != NULL) node = node->left;
    return node;
}

avlnode_t *avltree_delete(avlnode_t *at, avlnode_t *node, avltree_cmp_fn_t cmp_fn)
{
    if (at == NULL || node == NULL || cmp_fn == NULL) return at;

    if (cmp_fn(node, at) < 0)
    {
        at->left = avltree_delete(
                at->left,
                node, 
                cmp_fn);
    } else if (cmp_fn(node, at) > 0)
    {
        at->right = avltree_delete(
                at->right,
                node, 
                cmp_fn);
    } else {
        if (at->left == NULL) return at->right;
        if (at->right == NULL) return at->left;

        avlnode_t *succ = find_min(at->right);

        avlnode_t *left = at->left;
        avlnode_t *right = at->right;

        right = avltree_delete(
                    right, 
                    succ, 
                    cmp_fn);

        succ->left = left;
        succ->right = right;

        at = succ;
    }

    return rebalance(at);
}


