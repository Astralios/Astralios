#pragma once

#include "fs/vfs.h"

#define DEV_NAME_MAX    64


typedef inode_ops_t devfs_ops_t;
typedef struct devfs_inode_t devfs_inode_t;
typedef struct devfs_inode_t
{
    devfs_inode_t *parent;
    char name[DEV_NAME_MAX];
} devfs_inode_t;

extern devfs_inode_t *root_devfs;

devfs_inode_t *devfs_register(devfs_inode_t *parent, char *name, devfs_ops_t *ops);


