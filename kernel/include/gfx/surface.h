#pragma once

#include <bootstub.h>
#include <stddef.h>

#define surface_pixels_count(surface) ((surface)->framebuffer->height * (surface)->framebuffer->width)
#define surface_height(surface) ((surface)->framebuffer->height)
#define surface_width(surface) ((surface)->framebuffer->width)

typedef struct gfx_surface_t
{
    fb_t*       framebuffer;
    uint32_t*   backbuffer;
} gfx_surface_t;

gfx_surface_t*  gfx_surface_create(fb_t *framebuffer);
void            gfx_surface_sync(gfx_surface_t *surface, size_t start, size_t end);
void            gfx_surface_put_pixel(gfx_surface_t *surface, size_t x, size_t y, uint32_t color);
void gfx_draw_rect(gfx_surface_t *surface, int x, int y, size_t width, size_t height, uint32_t color);

void gfx_surface_sync_chunk(
    gfx_surface_t *surface,
    int x,
    int y,
    size_t width,
    size_t height
);

