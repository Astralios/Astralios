#include <gfx/font.h>
#include <gfx/surface.h>
#include <terminal/fbtty.h>

#define fbtty_width(tty) ((tty)->surface->framebuffer->width)
#define SPACE_SIZE 1
#define FOREGROUND_COLOR 0xFF00

void fbtty_init(fbtty_t *tty, fb_t *fb)
{
    tty->surface = gfx_surface_create(fb);
    tty->cursor_x = 0;
    tty->cursor_y = 0;
    tty->font = &iso_font;
}

#define BACKGROUND_COLOR 0x0000

void fbtty_backspace(fbtty_t *tty)
{
    if (tty->cursor_x == 0 && tty->cursor_y == 0) return;

    if (tty->cursor_x == 0)
    {
        tty->cursor_y -= tty->font->height + SPACE_SIZE;
        tty->cursor_x = fbtty_width(tty) - (tty->font->width + SPACE_SIZE);
    }
    else
    {
        tty->cursor_x -= tty->font->width + SPACE_SIZE;
    }

    gfx_draw_rect(tty->surface, tty->cursor_x, tty->cursor_y, 
                  tty->font->width + SPACE_SIZE, tty->font->height, 
                  BACKGROUND_COLOR);

    gfx_surface_sync(tty->surface, tty->cursor_x + tty->cursor_y * fbtty_width(tty),
                     tty->cursor_x + fbtty_width(tty) * (tty->cursor_y + tty->font->height));
}
void fbtty_write_char(fbtty_t *tty, const char ch)
{
    if (tty->cursor_x + tty->font->width >= fbtty_width(tty))
    {
        tty->cursor_x = 0;
        tty->cursor_y += tty->font->height + SPACE_SIZE;
    }

    size_t prev_cursor_x = tty->cursor_x;

    gfx_draw_char(tty->surface, ch, tty->font, tty->cursor_x, tty->cursor_y, FOREGROUND_COLOR);
    tty->cursor_x += tty->font->width + SPACE_SIZE;

    gfx_surface_sync(tty->surface, prev_cursor_x + tty->cursor_y * fbtty_width(tty), tty->cursor_x + fbtty_width(tty) * (tty->cursor_y + tty->font->height));

}

void fbtty_write_str(fbtty_t *tty, const char *str)
{
    if (!str) return;
    const char *s = str;
    while (*s)
    {
        fbtty_write_char(tty, *s);
        s++;
    }
}
