/* Circular Linked List */
// NOTE: This implementation is a slightly modified version of that of the linux kernel itself.
// https://github.com/torvalds/linux/blob/b1bc554e009e3aeed7e4cfd2e717c7a34a98c683/tools/firewire/list.h
//
// Things like list_insert, list_append are all literally the exact same, with the exception 
// of list_remove which also closes in the array element removed since the linux version didn't clean it up
#pragma once

#include <stddef.h>
#include <stdbool.h>

typedef struct clist_t clist_t;
typedef struct clist_t 
{
    clist_t *next, *prev;
} clist_t;

static inline void clist_init(clist_t *list) {
    list->next = list;
    list->prev = list;
}

static inline clist_t *clist_last(clist_t *list) {
    return list->prev != list ? list->prev : NULL;
}

static inline clist_t *clist_next(clist_t *list) {
    return list->next != list ? list->next : NULL;
}


static inline void list_insert(clist_t *new, clist_t *link) {
    new->prev = link->prev;
    new->next = link;
    new->prev->next = new;
    new->next->prev = new;
}

static inline void list_append(clist_t *new, clist_t *into) {
    list_insert(new, into->next);
}

static inline void list_remove(clist_t *list) {
    list->prev->next = list->next;
    list->next->prev = list->prev;
    list->next = list;
    list->prev = list;
}

static inline bool list_empty(clist_t *list)
{
    return list->next == list;
}


