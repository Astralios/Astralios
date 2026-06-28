#pragma once
#include <stdint.h>

typedef enum inode_kind_t: uint8_t {
    INODE_KIND_DIR,
    INODE_KIND_FILE,
} inode_kind_t;



