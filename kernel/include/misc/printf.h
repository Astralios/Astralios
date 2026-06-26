#pragma once

#include <stdarg.h>

void _printf(void (*put)(int c), void (*puts)(const char *s), const char *fmt, va_list args);

