#pragma once
#include <gfx/surface.h>
#include <stddef.h>
#include <stdint.h>

typedef struct gfx_font_t 
{
    size_t width;
    size_t height;
    const uint8_t *glyphs;
} gfx_font_t;

extern const gfx_font_t iso_font;

void gfx_draw_char(gfx_surface_t *surface, const char ch, const gfx_font_t *font, size_t x, size_t y, uint32_t color);

