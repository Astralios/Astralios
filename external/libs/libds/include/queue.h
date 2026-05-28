#pragma once

#include <stdint.h>
typedef struct queue_t
{
    uintptr_t tail, head;
    void *buf;
} queue_t;

