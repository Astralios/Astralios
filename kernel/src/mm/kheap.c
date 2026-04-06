#include "mm/kheap.h"
#include "arch/x86_64/def.h"
#include "arch/x86_64/mm/paging.h"
#include "bootstub.h"
#include "mm/pmm/pmm.h"
#include "mm/vmm/vmm.h"
#include "misc/todo.h"

// static vmm_t kheap_vmm = { .name = "kheap" };

void kheap_init(page_table_t *pt, vaddr_t begin, size_t pages)
{
    todo(kheap_init);
    // kheap_vmm.pt = pt;
    // vmm_region_t *heap_region = paddr_ptr(pmm_alloc(sizeof(vmm_region_t)));
    // heap_region->name = kheap_vmm.name;
    // heap_region->addr = begin;
    // heap_region->pages = pages;
    // heap_region->state = FREE;
    // heap_region->next = NULL;
    // heap_region->prev = NULL;
    // kheap_vmm.regions = heap_region;
}

void* kmmalloc(size_t pages, vmm_area_state_t state)
{
    todo(kmalloc);
    // return (void*)vmm_alloc(&kheap_vmm, state, kheap_vmm.name, pages, PAGE_P | PAGE_RW);
}

void* kheap_alloc(size_t pages)
{
    todo(kheap_alloc);
    // return (void*)vmm_alloc(&kheap_vmm, ALLOCATED, kheap_vmm.name, pages, PAGE_RW | PAGE_P);
}

void kheap_free(void* addr)
{
    todo(kheap_free);
    // vmm_free(&kheap_vmm, (vaddr_t)addr);
}

void kheap_debug(void)
{
    todo(kheap_debug);
    // vmm_debug(DEBUG_VMM_REGION_PRINT, kheap_vmm.regions);
}
