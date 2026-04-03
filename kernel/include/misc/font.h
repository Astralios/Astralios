#pragma once

#include "bootstub.h"
#include "drivers/fb.h"
#include <stdint.h>

typedef struct font_t font_t;
typedef struct font_t {
    size_t width, height;
    const void *chars;
    void (*draw_char)(const font_t *font, const char c, uint32_t color, fb_driver_t *fb_driver, size_t x, size_t y);
} font_t;

extern const font_t iso_font;

