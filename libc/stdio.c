#include "stdio.h"
#include "vendor/buf.h"
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

buf_writer_t(char, char_buf_writer_t);

size_t snprintf(char *buf, size_t size, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    char_buf_writer_t buf_writer = { .pos = 0, .buf = buf, .cap = size };
    size_t total_len = 0; 
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 's':
            {
                const char *value = va_arg(args, const char *);
                size_t len = strlen(value);
                buf_writer_write_all(&buf_writer, value, len);
                total_len += len;
                break;
            }            
            case 'd':
            {
                int value = va_arg(args, int);

                if (value == 0)
                {
                    buf_writer_write(&buf_writer, '0');
                    total_len++;
                    break;
                }

                if (value < 0)
                {
                    buf_writer_write(&buf_writer, '-');
                    total_len++;
                    value = -value;
                }

                char str[64] = {0};
                int j = 0;
                while (value > 0)
                {
                    char digit = value % 10 + '0';
                    value /= 10;
                    str[j++] = digit;
                }

                bool ignore = true;
                for (int i = 63; i >= 0; i--)
                {
                    char c = str[i];
                    if (c == 0 && ignore) continue;
                    total_len++;
                    buf_writer_write(&buf_writer, c);
                }

                break;
            }
            }
        } else
        {
            buf_writer_write(&buf_writer, *fmt);
            total_len++;
        }
        fmt++;
    }
    
    buf_writer_write(&buf_writer, 0);
    va_end(args);
    return total_len;
}

int sprintf(char *str, const char *fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int total_len = 0; 
    while (*fmt)
    {
        if (*fmt == '%')
        {
            fmt++;
            switch (*fmt)
            {
            case 's':
            {
                const char *value = va_arg(args, const char *);
                size_t len = strlen(value);
                memcpy(str + total_len, value, len); 
                total_len += len;
                break;
            }            
            case 'd':
            {
                int value = va_arg(args, int);

                if (value == 0)
                {
                    str[total_len++] = '0';
                    break;
                }

                if (value < 0)
                {
                    str[total_len++] = '-';
                    value = -value;
                }

                char buf[64] = {0};
                int j = 0;
                while (value > 0)
                {
                    char digit = value % 10 + '0';
                    value /= 10;
                    buf[j++] = digit;
                }

                bool ignore = true;
                for (int i = 63; i >= 0; i--)
                {
                    char c = buf[i];
                    if (c == 0 && ignore) continue;
                    str[total_len++] = c;
                }

                break;
            }
            }
        } else
        {
            str[total_len++] = *fmt;
        }
        fmt++;
    }

    str[total_len] = '\0';
    va_end(args);
    return total_len;
}
