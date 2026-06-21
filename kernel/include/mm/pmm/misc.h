#pragma once

#include <kernel.h>
#include <mm/pmm/pmm.h>

static inline vaddr_t pmm_vpalloc(size_t num_pages)
{
    return to_vaddr(pmm_palloc(num_pages));
}

static inline vaddr_t pmm_valloc(size_t size)
{
    return to_vaddr(pmm_alloc(size));
}

