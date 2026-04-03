#include "devices/tty.h"
#include "drivers/fb.h"
#include <stdarg.h>

void tty_putchar(tty_t *tty, const char c)
{
    tty->font->draw_char(tty->font, c, WHITE, tty->fb_driver, tty->cx, tty->cy);
    tty->cx += tty->font->width + 1;
}

void tty_putchars(tty_t *tty, const char *s)
{
    while (*s) tty_putchar(tty, *(s++)); 
    fb_driver_sync(tty->fb_driver, 0, to_fb_idx(tty->fb_driver->fb, tty->fb_driver->fb->width - 1, tty->fb_driver->fb->height - 1));
}

void tty_printf(tty_t *tty, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);


    va_end(args);
}
