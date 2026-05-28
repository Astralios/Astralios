#pragma once
#include "arch/x86_64/mm/paging.h"
#include <stddef.h>

void vheap_init(void);
void *vmalloc(size_t size);
void *vmalloc_pf(size_t size, page_flags_t flags);
void vfree(void *ptr);

