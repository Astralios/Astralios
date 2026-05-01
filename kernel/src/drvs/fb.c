#include <stdint.h>
#include <string.h>

#include <bootstub.h>
#include <drvs/fb.h>

fb_driver_t *fb_driver_init(fb_t *fb)
{
    // FIXME: Replace with actual kmalloc
    fb_driver_t *fb_driver = NULL;
    fb_driver->fb = fb;
    size_t bf_size = (fb->height * (fb->pitch / 4)) * sizeof(uint32_t);
    fb_driver->backbuffer = NULL;
    return fb_driver;
}

void fb_driver_sync(fb_driver_t *fb_driver, size_t from, size_t to)
{
    if (
        from < fb_driver->fb->width * fb_driver->fb->height 
        &&
        to < fb_driver->fb->width * fb_driver->fb->height
    ) {
        memcpy((uint32_t*)fb_driver->fb->addr + from, fb_driver->backbuffer + from, (to - from) * sizeof(uint32_t));
    }
}

void fb_driver_set_pixel(fb_driver_t *fb_driver, size_t x, size_t y, uint32_t color)
{
    if (x < fb_driver->fb->width && y < fb_driver->fb->height)
    {
        fb_driver->backbuffer[y * (fb_driver->fb->pitch / 4) + x] = color;
    }
}

void fb_driver_fill_entirely(fb_driver_t *fb_driver, uint32_t color)
{
    memset_u32(fb_driver->backbuffer, color, fb_driver->fb->height * fb_driver->fb->width); 
    // fb_driver_sync(fb_driver, 0, to_fb_idx(fb_driver->fb, fb_driver->fb->width - 1, fb_driver->fb->height - 1));
}

void fb_driver_draw_rect(fb_driver_t *fb_driver, size_t x, size_t y, size_t w, size_t h, uint32_t color)
{
    for (size_t py = y; py < y + h; ++py)
    {
        for (size_t px = x; px < x + w; ++px)
        {
            fb_driver_set_pixel(fb_driver, px, py, color);
        }
    }
    // fb_driver_sync(fb_driver, to_fb_idx(fb_driver->fb, x, y), to_fb_idx(fb_driver->fb, x + w, y + h));
}

