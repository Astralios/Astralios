#include "ringbuf.h"
#include <stdbool.h>
#include <string.h>

bool ringbuf_init(ringbuf_t *ringbuf, size_t cap, size_t size, ringbuf_mode_t mode, alloc_fn alloc_fn)
{
    ringbuf->len = 0;
    ringbuf->head = 0;
    ringbuf->tail = 0;
    ringbuf->size = size;
    ringbuf->cap = cap;
    ringbuf->mode = mode;
    ringbuf->buf = alloc_fn(cap * size);
    if (!ringbuf->buf) return false;
    return true;
}


bool ringbuf_empty(ringbuf_t *rb)
{
    return rb->len == 0 ? true : false;
}

bool ringbuf_write(ringbuf_t *rb, const void *buf)
{
    if (rb->mode == RINGBUF_MODE_OVERWRITE)
    {
        memcpy(rb->buf + rb->tail * rb->size, buf, rb->size);

        if (rb->len == rb->cap) {
            rb->head = (rb->head + 1) % rb->cap;
        } else {
            rb->len++;
        }

        rb->tail = (rb->tail + 1) % rb->cap;
        return true;
    } else if (rb->mode == RINGBUF_MODE_BLOCKING)
    {
        if (rb->len == rb->cap) return false;

        memcpy(rb->buf + rb->tail * rb->size, buf, rb->size);
        rb->tail = (rb->tail + 1) % rb->cap;
        rb->len++;

        return true;
    }

    return false;
}

bool ringbuf_read(ringbuf_t *rb, void *buf)
{
    if (rb->len == 0) return false;
    memcpy(buf, rb->buf + rb->head * rb->size, rb->size);

    rb->head = (rb->head + 1) % rb->cap;
    rb->len--;

    return true;
}


