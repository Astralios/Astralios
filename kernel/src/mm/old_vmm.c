// #include "mm/vmm.h"
// #include "arch/x86_64/cpu.h"
// #include "mm/pmm.h"
// #include "misc/debug.h"
// #include "misc/helpers.h"
// #include "vendor/list.h"
// #include <stdarg.h>
// #ifdef __ARCH_X86_64__
// #include "arch/x86_64/def.h"
// #include "arch/x86_64/mm/paging.h"
// #endif

// extern page_table_t *pml4;

// static const char* vmm_region_state_str[VMM_REGION_STATE_COUNT] = {
//     [FREE] = "free",
//     [ALLOCATED] = "allocated",
//     [LAZILY_ALLOCATED] = "lazily allocated",
// };

// static const char* vmm_flag_str[VMM_FLAGS_COUNT] = {
//     [VMM_WRITEABLE] = "Writable",
//     [VMM_EXECUTABLE] = "Executable",
//     [VMM_USER_ACCESSIBLE] = "User Accessible",
//     [VMM_UNCACHEABLE] = "Uncacheable",
//     [VMM_FRAMEBUFFER_CACHED] = "Framebuffer Cached",
//     [VMM_ZEROED] = "Zeroed",
// };

// const char *vmm_region_state_to_str(vmm_region_state_t state)
// {
//     return vmm_region_state_str[state];
// }

// const char *vmm_flag_to_str(vmm_flag_t flag)
// {
//     return vmm_flag_str[flag];
// }

// void vmm_debug(vmm_debug_mode_t mode, ...)
// {
//     va_list args;
//     va_start(args, mode);

//     switch (mode) {
//         case DEBUG_VMM_REGION_PRINT:
//         {
//             vmm_region_t *list = va_arg(args, vmm_region_t*);
//             vmm_region_t *curr = list;
//             while (curr)
//             {
//                 srdebug(vmm_debug, "name: %s; addr: %x; flags: %x; pages: %zu; state: %s; next: %x; prev: %x", curr->name, curr->addr, curr->flags, curr->pages, vmm_region_state_str[curr->state], curr->next, curr->prev);
//                 curr = curr->next;
//             }
//             break;
//         }
//     }

//     va_end(args);
// }

// #define lazily_vaddr(reg)                        \
//     ({                                              \
//         (lazilly_alloc_list_t){ .region = reg };  \
//     })

// list_t lazy_vaddr_list = { .next = &lazy_vaddr_list, .prev = &lazy_vaddr_list };

// vaddr_t vmm_alloc(vmm_t *vmm, vmm_region_state_t state, char *name, size_t pages, int flags)
// {
//     if (pages == 0) return 0;
//     if (name == NULL) name = "Unnamed";
    
//     vmm_region_t *curr = vmm->regions;
//     while (curr)
//     {
//         if (curr->pages >= pages && curr->state == FREE)
//         {
//             if (curr->pages > pages)
//             {
//                 vmm_region_t *new_region = paddr_ptr(pmm_alloc(sizeof(vmm_region_t)));

//                 new_region->flags = curr->flags;
//                 new_region->pages = curr->pages - pages;
//                 new_region->name = curr->name;
//                 new_region->state = curr->state;
//                 new_region->next = curr->next;
//                 new_region->prev = curr;
//                 new_region->addr = curr->addr + pages * PAGE_SIZE;

//                 curr->next = new_region;
//                 curr->flags = flags;
//                 curr->state = state;
//                 curr->name = name;
//                 curr->pages = pages;
      
//                 if (state == ALLOCATED)
//                 {
//                     for (size_t i = 0; i < curr->pages; i++)
//                     {
//                         map_to_pt(vmm->pt, pmm_palloc(), curr->addr + i * PAGE_SIZE, flags);
//                     }
//                 } else if (state == LAZILY_ALLOCATED)
//                 {
//                     lazilly_alloc_list_t new = lazily_vaddr(curr);
//                     list_append(&lazy_vaddr_list, &new.list);
//                 }

//                 return curr->addr;
//             } else if (curr->pages == pages)
//             {
//                 curr->state = state;
//                 curr->flags = flags;
//                 curr->name = name;
//                 if (state == ALLOCATED)
//                 {
//                     for (size_t i = 0; i < curr->pages; i++)
//                     {
//                         map_to_pt(vmm->pt, pmm_palloc(), curr->addr + i * PAGE_SIZE, flags);
//                     }
//                 } else if (state == LAZILY_ALLOCATED)
//                 {
//                     lazilly_alloc_list_t new = lazily_vaddr(curr);
//                     list_append(&lazy_vaddr_list, &new.list);
//                 }

//                 return curr->addr;
//             }
//         }
//         curr = curr->next;
//     }
//     return 0;
// }

// void vmm_free(vmm_t *vmm, vaddr_t vaddr)
// {
//     if (vaddr == 0) return;
    
//     vmm_region_t *curr = vmm->regions;
//     while (curr)
//     {
//         if (curr->addr == vaddr) break;
//         curr = curr->next;
//     }
//     if (curr == NULL) return;
//     curr->state = FREE;
//     curr->name = vmm->name;
//     curr->flags = 0;

//     for (size_t i = 0; i < curr->pages; i++)
//     {
//         unmap_from_pt(vmm->pt, curr->addr + i * PAGE_SIZE);
//         invplg(curr->addr);
//     }

//     if (curr->prev && curr->prev->state == FREE)
//     {
//         vmm_region_t *old = curr->prev;
//         curr->addr = old->addr;
//         curr->pages += old->pages;
//         curr->prev = old->prev;
//         if (curr->prev)
//             curr->prev->next = curr;
//         pmm_free((paddr_t)old, sizeof(vmm_region_t));
//     }

//     if (curr->next && curr->next->state == FREE)
//     {
//         vmm_region_t *old = curr->next;
//         curr->pages += old->pages;
//         curr->next = old->next;
//         if (curr->next)
//             curr->next->prev = curr;
//         pmm_free*((paddr_t)old, sizeof(vmm_region_t));
//     }
// }
