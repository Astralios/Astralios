#pragma once

#include <stddef.h>

#define ringbuf_t(type, name) typedef struct name { type* buf; size_t cap, head, tail; } name
#define ringbuf_full(rb) (((rb)->head + 1) % (rb)->cap == (rb)->tail)

#define ringbuf_read(rb)                            \
    ({                                              \
        size_t idx = (rb)->tail;                    \
        (rb)->tail = (idx + 1) % (rb)->cap;         \
        (rb)->buf[idx];                             \
    })

#define ringbuf_write(ringbuf, val)                                     \
    do {                                                                \
        if (!ringbuf_full(ringbuf))                                     \
        {                                                               \
            (ringbuf)->buf[(ringbuf)->head] = val;                      \
            (ringbuf)->head = ((ringbuf)->head + 1) % (ringbuf)->cap;   \
        }                                                               \
    } while (0)


