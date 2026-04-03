#pragma once

#include <stddef.h>

size_t snprintf(char *buf, size_t size,
             const char* format, ...);

int sprintf(char *str, const char *fmt, ...);

