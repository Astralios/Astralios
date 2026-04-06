#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct strview_t 
{
    const char *str;
    size_t len;
} strview_t;

#define strview(s, l)   (strview_t){ .str = (s), .len = (l) }
#define strview_end(s)  (s.str + s.len)


