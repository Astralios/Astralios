#include "mm/slab.h"

#define slab_bufctl(slab) ((uint32_t*)(slab + 1))
#define slab_mem(cache) ((cache)->obj_size * (cache)->obj_per_slab + sizeof(slab_t) + sizeof(uint32_t))


