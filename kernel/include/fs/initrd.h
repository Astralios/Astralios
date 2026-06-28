#pragma once
#include <fs/vfs.h>
void initrd_init(void);
int initrd_get(const char *name, inode_t **ret);

