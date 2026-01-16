#pragma once

#include "devices/serial.h"

#ifdef __DEBUG__
#define srdebug(from, fmt, ...)       srprintf("[%s]: " fmt "\n", #from, ##__VA_ARGS__)
#define dsrdebug(from, fmt, ...) srprintf("[%s(%s:%d)]: " fmt "\n", #from, __FILE__, __LINE__, ##__VA_ARGS__)
#define debug(fmt, ...)          srprintf("[DEBUG]: " fmt "\n", ##__VA_ARGS__)
#define frdebug(from, fmt, ...)  srprintf("[%s]: " fmt "\n", from, ##__VA_ARGS__)
#else
#define srdebug(from, ...)
#define dsrdebug(from, fmt, ...)
#define debug(fmt, ...)
#define frdebug(from, fmt, ...)
#endif
