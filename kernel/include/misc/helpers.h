#pragma once

#include <stddef.h>

#define ALIGN_UP(address, alignment) (((address) + (alignment - 1)) & ~((alignment) - 1))
#define ALIGN_DOWN(address, alignment) ((address) & ~((alignment) - 1))
#define BYTES_TO_FULL_PAGES(bytes) (ALIGN_UP(bytes, 4096) / 4096)
#define CONTAINER_OF(type, ptr) ((type)(((char*)ptr) - offsetof(type, ptr)))

