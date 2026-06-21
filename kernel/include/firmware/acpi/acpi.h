#pragma once

#include <stdint.h>

typedef struct 
{
    char        signature[8];
    uint8_t     checksum;
    char        oem_id[6];
    uint8_t     revision;
    uint32_t    rsdt_addr;
} __attribute__((packed)) rsdp_t;

typedef struct
{
    char        signature[8];
    uint8_t     checksum;
    char        oem_id[6];
    uint8_t     revision;
    uint32_t    rsdt_addr;

    uint32_t    length;
    uint64_t    xsdt_addr;
    uint8_t     extended_checksum;
    uint8_t     reserved[3];
} __attribute__((packed)) xsdp_t;

typedef struct 
{
    char        signature[4];
    uint32_t    length;
    uint8_t     revision;
    uint8_t     checksum;
    char        oem_id[6];
    char        oem_table_id[8];
    uint32_t    oem_revision;
    uint32_t    creator_id;
    uint32_t    creator_revision;
} __attribute__((packed)) acpi_sdt_header;

typedef struct
{
    acpi_sdt_header header;
    uint32_t sdts[];
} __attribute__((packed)) rsdt_t;

typedef struct
{
    acpi_sdt_header header;
    uint64_t sdts[];
} __attribute__((packed)) xsdt_t;

void acpi_init(void);

