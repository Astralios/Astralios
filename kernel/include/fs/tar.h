#pragma once

#include "fs/vfs.h"
#define TAR_FILE '0'
#define TAR_DIR  '5'

// struct stolen from: https://github.com/Mellurboo/Bleed-Kernel/blob/main/kernel/include/fs/archive/tar.h
typedef struct tar_header_t {
    char name[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char checksum[8];
    char typeflag;
    char linkname[100];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[155];
    char padding[12];
} __attribute__((packed)) tar_header_t;

void tar_extract(void *tar);

