#pragma once

#define ALIGN_UP(address, alignment) (((address) + (alignment - 1)) & ~((alignment) - 1))
#define ALIGN_DOWN(address, alignment) ((address) & ~((alignment) - 1))
