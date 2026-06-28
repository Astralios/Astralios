#pragma once

#include <stdint.h>
#include <stddef.h>
typedef struct fb_t
{
    uint64_t addr;
    uint64_t width;
    uint64_t height;
    uint64_t pitch;
    uint8_t memory_model;
    uint16_t bpp;
    uint8_t red_mask_size;
    uint8_t red_mask_shift;
    uint8_t green_mask_size;
    uint8_t green_mask_shift;
    uint8_t blue_mask_size;
    uint8_t blue_mask_shift;
} fb_t;

typedef struct fbs_t fbs_t;
typedef struct fbs_t {
    void (*get_fb)(fbs_t *fbs, fb_t *fb, size_t i);
    size_t num_fbs;
} fbs_t;

static inline uint32_t fb_colourFromRGB(fb_t *fb, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t colour = 0;
    colour |= (r * (fb->red_mask_size) / 255) << fb->red_mask_shift;
    colour |= (g * (fb->green_mask_size) / 255) << fb->green_mask_shift;
    colour |= (b * (fb->blue_mask_size) / 255) << fb->blue_mask_shift;
    return colour;
}
