#pragma once

#include "bootstub.h"
#include <stdint.h>

#define WHITE 0xFFFFFF
#define BLACK 0x000000
#define RED 0xFF0000
#define LIME 0x00FF00
#define BLUE 0x0000FF
#define YELLOW 0xFFFF00
#define GRAY 0x808080
#define GREEN 0x008000
#define PURPLE 0x800080

#define to_fb_idx(fb, x, y) ((y) * ((fb)->pitch / 4) + (x))

typedef struct fb_driver_t {
    fb_t *fb;
    uint32_t *backbuffer;
} fb_driver_t;

fb_driver_t*    fb_driver_init(fb_t *fb);
void            fb_driver_sync(fb_driver_t *fb_driver, size_t from, size_t to);
void            fb_driver_set_pixel(fb_driver_t *fb_driver, size_t x, size_t y, uint32_t color);
void            fb_driver_fill_entirely(fb_driver_t *fb_driver, uint32_t color);
void fb_driver_draw_rect(fb_driver_t *fb_driver, size_t x, size_t y, size_t w, size_t h, uint32_t color);


