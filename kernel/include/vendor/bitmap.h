#pragma once

#include <stdint.h>
#include <stddef.h>

typedef struct bitmap_t
{
    size_t  size;
    uint8_t map[];
} bitmap_t;


static inline void bitmap_set(bitmap_t *bitmap, size_t idx)
{
    size_t byte_idx = idx / 8;
    size_t bit_idx  = idx % 8;
    bitmap->map[byte_idx] |= (0b1 << bit_idx);    
}

static inline void bitmap_unset(bitmap_t *bitmap, size_t idx)
{
    size_t byte_idx = idx / 8;
    size_t bit_idx = idx % 8;
    bitmap->map[byte_idx] &= ~(0b1 << bit_idx);
}

static inline uint8_t bitmap_bit(bitmap_t *bitmap, size_t idx)
{
    size_t byte_idx = idx / 8;
    size_t bit_idx  = idx % 8;
    return (bitmap->map[byte_idx] >> bit_idx) & 0b1;
}

static size_t bitmap_find_contiguous_range(bitmap_t *bitmap, size_t contiguous_bits_needed)
{
    size_t len = 0;
    for (size_t i = 0; i < bitmap->size; i++)
    {
        if (bitmap_bit(bitmap, i) & 0b1) len = 0;
        else len++;
        if (len == contiguous_bits_needed) return (i + 1 - contiguous_bits_needed);
    }
    return (size_t)(-1);
}

static size_t bitmap_find_contiguous_range_from(bitmap_t *bitmap, size_t from, size_t contiguous_bits_needed)
{
    size_t len = 0;
    for (size_t i = from; i < bitmap->size; i++)
    {
        if (bitmap_bit(bitmap, i) & 0b1) len = 0;
        else len++;
        if (len == contiguous_bits_needed) return (i + 1 - contiguous_bits_needed);
    }
    return (size_t)(-1);
}

static void bitmap_set_range(bitmap_t *bitmap, size_t idx, size_t size)
{
    if (size + idx >= bitmap->size) return;
    for (size_t i = idx; i < idx + size; i++) bitmap_set(bitmap, i);
}

static void bitmap_unset_range(bitmap_t *bitmap, size_t idx, size_t size)
{
    if (size + idx >= bitmap->size) return;
    for (size_t i = idx; i < idx + size; i++) bitmap_unset(bitmap, i);
}
