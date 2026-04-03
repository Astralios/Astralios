#pragma once

#include <stddef.h>
#include "drivers/fb.h"
#include "misc/font.h"

typedef struct tty_t {
    fb_driver_t *fb_driver;
    size_t cx, cy;
    const font_t *font;
} tty_t;

void    tty_putchar(tty_t *tty, const char c);
void    tty_putchars(tty_t *tty, const char *s);
void    tty_printf(tty_t *tty, const char *fmt, ...);
