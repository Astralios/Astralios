#pragma once
#include <stddef.h>
#define buf_writer_t(type, name) typedef struct name { size_t pos; size_t cap; type *buf; } name;
#define buf_writer_write(buf_writer, val)                                                       \
do {                                                                                            \
    if (!(buf_writer)->buf) break;                                                              \
    if ((buf_writer)->pos < (buf_writer)->cap) (buf_writer)->buf[(buf_writer)->pos++] = val;    \
} while (0)                                                                                     \

#define buf_writer_write_all(buf_writer, vals, amount)                  \
do {                                                                    \
    if (!(buf_writer)->buf) break;                                      \
    size_t i = 0;                                                       \
    size_t space = (buf_writer)->cap - (buf_writer)->pos;               \
    if (amount > space) amount = space;                                 \
    for (i = 0; i < amount; i++)                                        \
        (buf_writer)->buf[(buf_writer)->pos++] = (vals)[i];             \
} while (0)                                                             \

