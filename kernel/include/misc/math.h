#pragma once
#include <stdbool.h>

typedef enum sign_t: bool { NEGATIVE, POSITIVE } sign_t;

#define sign(a)     ((a) > 0 ? POSITIVE : NEGATIVE)
#define modulus(a)  ((a) >= 0 ? (a) : -(a))
#define mod(a, b)   ((((a) % (b)) + (b)) % (b))

