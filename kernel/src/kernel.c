#include "arch/x86_64/mem/paging.h"
#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#include "arch/x86_64/gdt/gdt.h"
#include "arch/x86_64/idt/idt.h"
#include "arch/x86_64/tss.h"
#include "arch/x86_64/cpu.h"
// #include "arch/x86_64/mem/paging.h"
#endif

#include "bootstub.h"
#include "devices/serial.h"
#include "misc/debug.h"
#include "string.h"
#include "mem/pmm.h"

kernel_params_t *kernel_params = NULL;
// page_table_t *pml4 = NULL;
// static module_t *get_module(const char *path)
// {
//     for (size_t i = 0; i < kernel_params->modules.modules_count; i++)
//     {
//         if (strcmp(path, kernel_params->modules.modules[i].path))
//         {
//             return &kernel_params->modules.modules[i];
//         }
//     }
    
//     return NULL;
// }

void kmain(kernel_params_t *params)
{
    kernel_params = params;
    serial_init();
    gdt_init();
    idt_init();
    tss_init();
    pmm_init();    
    paging_init();

    void *a = (void*)to_vaddr(pmm_alloc(3));
    pmm_free(to_paddr(cast_vaddr(a)), 3); 

    hcf();
}
