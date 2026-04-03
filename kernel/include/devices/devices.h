#pragma once

#include "fs/vfs.h"
#include "misc/strview.h"

void devfs_init()
{

}

static int devfs_create(inode_t *dir, const char *name, size_t name_len, inode_kind_t kind, inode_t **ret);

inode_ops_t devfs_ops = {
    .create = devfs_create,
};


static int devfs_create(inode_t *dir, const char *name, size_t name_len, inode_kind_t kind, inode_t **ret)
{
    
}


