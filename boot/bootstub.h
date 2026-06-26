#pragma once

#include <fb.h>
#include <modules.h>
#include <stdint.h>
#include <stddef.h>

enum
{
    MEMMAP_RESERVED,
    MEMMAP_ACPI_RECLAIMABLE,
    MEMMAP_EXECUTABLE_AND_MODULES,
    MEMMAP_ACPI_NVS,
    MEMMAP_BAD_MEMORY,
    MEMMAP_BOOTLOADER_RECLAIMABLE,
    MEMMAP_FRAMEBUFFER,
    MEMMAP_USABLE,
    MEMMAP_UNKNOWN,
    MEMMAP_RESERVED_MAPPED,
    MEMMAP_ENTRY_TYPE_COUNT,
};

typedef struct memmap_entry_t
{
    uint64_t base;
    uint64_t length;
    uint64_t type;
} memmap_entry_t;

typedef struct memmap_t
{
    memmap_entry_t* entries;
    size_t          num_entries;
} memmap_t;

typedef struct krnl_map_t
{
    uint64_t physical_base;
    uint64_t virtual_base;
} krnl_map_t;

typedef struct acpi_rsdp_t
{
    uint64_t addr;
    uint64_t revision;
} acpi_rsdp_t;

typedef struct bootloader_ctx_t
{
    uint64_t    hhdm;
    modules_t   modules;
    krnl_map_t  krnl_map;
    acpi_rsdp_t acpi_rsdp;
    memmap_t    memmap;
    fb_t*       fbs;
} bootloader_ctx_t;

