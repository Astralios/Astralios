#include <stddef.h>

#include "bootstub.h"
#include "string.h"

#define LIMINE_API_REVISION 3
#include "limine.h"

#define MAX_HEAP_SIZE 1024 * 16

static char heap[MAX_HEAP_SIZE];
static uint32_t heap_head = 0;

void* heap_alloc(size_t size) 
{
    size = (size + 0xF) & ~0xF;
    if (!size || size > MAX_HEAP_SIZE - heap_head) return NULL;    
    heap_head += size;
    return (void*)(heap + heap_head - size);
}

static kernel_params_t kernel_params;

__attribute__((used, section(".limine_requests")))
static volatile LIMINE_BASE_REVISION(3);
__attribute__((used, section(".limine_requests_start")))
static volatile LIMINE_REQUESTS_START_MARKER;
__attribute__((used, section(".limine_requests_end")))
static volatile LIMINE_REQUESTS_END_MARKER;

__attribute__((used, section(".limine_requests"))) 
static volatile struct limine_hhdm_request hhdm_request = {
    .id = LIMINE_HHDM_REQUEST,
    .revision = 3};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_memmap_request memmap_request = {
    .id = LIMINE_MEMMAP_REQUEST,
    .revision = 3};

__attribute__((used, section(".limine_requests"))) 
static volatile struct limine_executable_address_request kernel_address_request = {
    .id = LIMINE_EXECUTABLE_ADDRESS_REQUEST,
    .revision = 3};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 3};

__attribute__((used, section(".limine_requests")))
static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 3
};

static int from_limine_type(int type) 
{
    switch (type) 
    {
        case LIMINE_MEMMAP_USABLE: return MEMMAP_USABLE;
        case LIMINE_MEMMAP_RESERVED: return MEMMAP_RESERVED;
        case LIMINE_MEMMAP_ACPI_RECLAIMABLE: return MEMMAP_ACPI_RECLAIMABLE;
        case LIMINE_MEMMAP_EXECUTABLE_AND_MODULES: return MEMMAP_EXECUTABLE_AND_MODULES;
        case LIMINE_MEMMAP_ACPI_NVS: return MEMMAP_ACPI_NVS;
        case LIMINE_MEMMAP_BAD_MEMORY: return MEMMAP_BAD_MEMORY;
        case LIMINE_MEMMAP_BOOTLOADER_RECLAIMABLE: return MEMMAP_BOOTLOADER_RECLAIMABLE;
        case LIMINE_MEMMAP_FRAMEBUFFER: return MEMMAP_FRAMEBUFFER;
        default: return MEMMAP_UNKNOWN;
    }
}

static kernel_addr_t get_kernel_address(void) 
{
    kernel_addr_t kaddr;
    kaddr.physical_base = kernel_address_request.response->physical_base;
    kaddr.virtual_base = kernel_address_request.response->virtual_base;
    return kaddr;
}

static memmap_t get_memmap(void)
{
    memmap_t memmap;
    memmap.entry_count = memmap_request.response->entry_count;
    memmap.entries = (memmap_entry_t*)heap_alloc(memmap.entry_count * sizeof(memmap_entry_t));
    for (uint64_t i = 0; i < memmap.entry_count; i++)
    {
        memmap.entries[i].base = memmap_request.response->entries[i]->base;
        memmap.entries[i].length = memmap_request.response->entries[i]->length;
        memmap.entries[i].type = from_limine_type(memmap_request.response->entries[i]->type);
    }

    return memmap;
}

static fb_t *get_framebuffers(void)
{
    struct limine_framebuffer_response *res = framebuffer_request.response;
    fb_t *fbs = (fb_t*)heap_alloc(res->framebuffer_count * sizeof(fb_t));
    for (size_t i = 0; i < res->framebuffer_count; i++)
    {
        struct limine_framebuffer *fb = res->framebuffers[i];
        fbs[i].addr = (uint64_t)fb->address;
        fbs[i].width = fb->width;
        fbs[i].height = fb->height;
        fbs[i].bpp = fb->bpp;
        fbs[i].pitch = fb->pitch;
        fbs[i].memory_model = fb->memory_model;
        fbs[i].blue_mask_shift = fb->blue_mask_shift;
        fbs[i].red_mask_shift = fb->red_mask_shift;
        fbs[i].green_mask_shift = fb->green_mask_shift;
        fbs[i].red_mask_size = fb->blue_mask_size;
        fbs[i].blue_mask_size = fb->blue_mask_size;
        fbs[i].green_mask_size = fb->blue_mask_size;
    }
    return fbs;
}

static inline uint64_t get_modules_count(void)
{
    if (module_request.response) return module_request.response->module_count;
    return 0;
}

static module_t from_limine_file(struct limine_file *file)
{
    module_t module = {0};
    module.addr = file->address;
    module.path = file->path;
    module.size = file->size;
    return module;
}

static void get_modules(modules_t *modules)
{
    uint64_t modules_count = get_modules_count();
    if (modules_count == 0) return;
    modules->modules_count = modules_count;
    modules->modules = (module_t*)heap_alloc(sizeof(module_t) * modules_count);

    for (size_t i = 0; i < modules_count; i++)
    {
        modules->modules[i] = from_limine_file(module_request.response->modules[i]);
    }
}

static void load_kernel_params(void)
{
    kernel_params.hhdm = hhdm_request.response->offset; 
    kernel_params.kernel_addr = get_kernel_address();
    kernel_params.memmap = get_memmap();
    kernel_params.fbs = get_framebuffers();
    get_modules(&kernel_params.modules);
}

extern void kmain(kernel_params_t *params);
// static void (*kmain)(kernel_params_t *params);
void kstart(void) 
{
    load_kernel_params();

    // kmain = (void (*)(kernel_params_t*))get_elf_entry("../kernel/build/bin/astralisos");
    kmain(&kernel_params);
}