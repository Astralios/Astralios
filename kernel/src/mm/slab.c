#include "arch/x86_64/mm/paging.h"
#include "misc/debug.h"
#include "mm/vmm/vmm.h"
#include "vendor/list.h"
#include <kernel.h>
#include <mm/slab.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/def.h>
#endif

#define slab_bufctl(slab)   ((uint32_t*)(slab + 1))
#define slab_mem(cache)     ((cache)->obj_size * (cache)->obj_per_slab + sizeof(slab_t) + sizeof(uint32_t))

static bool cache_grow(cache_t *cache)
{
    vaddr_t slab_addr = vmm_palloc(krnl_ctx.krnl_vmm, align_up(slab_mem(cache), PAGE_SIZE) / PAGE_SIZE, PAGE_FLAG_READ_WRITE);
    if (slab_addr == 0)
    {
        srdebug(cache_grow, "Couldn't allocate a slab!");
        return false;
    }

    slab_t *slab = (slab_t*)slab_addr;

    slab->cache = cache;
    slab->mem = (vaddr_t)((uint32_t*)(slab_addr + sizeof(slab_t)) + cache->obj_per_slab);
    slab->free_idx = 0;

    for (size_t i = 0; i < cache->obj_per_slab - 1; ++i)
    {
        slab_bufctl(slab)[i] = i;
    }

    list_append((list_t*)slab, &cache->slabs_free);

    return true;
}

static slab_t *cache_select(cache_t *cache)
{
    slab_t *slab = NULL;
}

