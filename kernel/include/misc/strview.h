#pragma once
#include <stddef.h>
#include <stdint.h>

typedef struct strview_t 
{
    const char *str;
    size_t len;
} strview_t;

#define strview(_str, _len) (strview_t){ .str = (_str), .len = (_len) }
#define strview_end(strview) (strview.str + strview.len)


