#pragma once
#include <stddef.h>
#include <stdbool.h>

typedef void* (alloc_fn)(size_t size);

typedef enum {
    RINGBUF_MODE_BLOCKING,
    RINGBUF_MODE_OVERWRITE,
} ringbuf_mode_t;

typedef struct {
    void*           buf;
    size_t          size;
    size_t          cap;
    size_t          len;
    size_t          head;
    size_t          tail;
    ringbuf_mode_t  mode;
} ringbuf_t;

bool ringbuf_init(ringbuf_t *ringbuf, size_t cap, size_t size, ringbuf_mode_t mode, alloc_fn alloc_fn);
bool ringbuf_write(ringbuf_t *rb, const void *buf);
bool ringbuf_read(ringbuf_t *rb, void *buf);
bool ringbuf_empty(ringbuf_t *rb);

