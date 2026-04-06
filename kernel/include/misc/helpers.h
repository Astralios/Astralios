#pragma once
#include <stddef.h>

#define stringify(x) #x
#define concat(a, b) a##b

#define align_up(address, alignment)    (((address) + (alignment - 1)) & ~((alignment) - 1))
#define align_down(address, alignment)  ((address) & ~((alignment) - 1))
#define container_of(ptr, type, member) ((type *)((char *)(ptr) - offsetof(type, member)))
