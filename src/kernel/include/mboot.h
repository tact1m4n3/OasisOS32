#pragma once

#include <types.h>

#define MBOOT_MAGIC 0x2BADB002

typedef struct mboot_info {
    uint32_t flags;

    uint32_t mem_lower;
    uint32_t mem_upper;

    uint32_t boot_device;

    uint32_t cmd_line;
    
    uint32_t mods_count;
    uint32_t mods_addr;

    // ...
} mboot_info_t;

typedef struct mboot_module {
    uint32_t mod_start;
    uint32_t mod_end;

    uint32_t cmd_line;
    
    uint32_t padding;
} mboot_module_t;
