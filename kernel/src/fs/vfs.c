#include "mm/slab.h"
#include <stddef.h>
#include <string.h>

#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <misc/err.h>
#include <misc/strview.h>

inode_t *vfs_root = NULL;
cache_t* inode_cache = NULL;

long inode_read(inode_t *inode, void *buf, size_t count, size_t offset)
{
    if (inode->ops->read == NULL) return -UNIMPLEMENTED;
    return inode->ops->read(inode, buf, count, offset);
}

long inode_write(inode_t *inode, const void *buf, size_t count, size_t offset)
{
    if (inode->ops->write == NULL) return -UNIMPLEMENTED;
    return inode->ops->write(inode, buf, count, offset);
}

int inode_lookup(inode_t *dir, const char *name, size_t name_len, inode_t **ret)
{
    if (dir->ops->lookup == NULL) return -UNIMPLEMENTED;
    return dir->ops->lookup(dir, name, name_len, ret);
}

int inode_create(inode_t *dir, const char *name, size_t name_len, inode_kind_t kind, inode_t **ret)
{
    if (dir->ops->create == NULL) return -UNIMPLEMENTED;
    return dir->ops->create(dir, name, name_len, kind, ret);
}

int fs_mount(const fs_t *fs, inode_t **root)
{
    if (fs->mount == NULL) return -UNIMPLEMENTED;
    return fs->mount(root);
}

static path_t parent_path(const path_t *path)
{
    const char *end = strview_end(path->strview) - 1;
    while (end > path->strview.str && *end == '/') end--;
    while (end > path->strview.str && *end != '/') end--;
    return (path_t){
        .root = path->root,
        .from = path->from,
        .strview = strview(path->strview.str, end - path->strview.str + 1),
    };
}

static strview_t basename_of_path(const path_t *path)
{
    const char *end = strview_end(path->strview) - 1; 
    while (end > path->strview.str && *end == '/') end--;
    const char *str_end = end;
    while (end > path->strview.str && *end != '/') end--;
    return strview(end + 1, str_end - end);
}

path_t vfs_path_from_abs(const char *cstr)
{
    return (path_t){
        .root = vfs_root,
        .from = vfs_root,
        .strview = strview(cstr, strlen(cstr)),
    };
}
        
int vfs_lookup(const path_t *path, inode_t **ret)
{
    inode_t *curr_inode = path->from;
    const char *start = path->strview.str,
               *end   = strview_end(path->strview);
    
    while (start < end)
    {
        while (start < end && *start == '/') start++;
        if (start < end)
        {
            const char *name_start = start;
            while (start < end && *start != '/') start++;
            int e = inode_lookup(curr_inode, name_start, start - name_start, &curr_inode);
            if (e < 0) return e;
        }
    }

    *ret = curr_inode;
    return SUCCESS;
}

int vfs_create(const path_t *path, inode_kind_t kind, inode_t **ret)
{
    path_t parent = parent_path(path);
    inode_t *parent_inode = NULL;

    int e = vfs_lookup(&parent, &parent_inode);
    if (e < 0) return e;
    
    strview_t basename = basename_of_path(path);
    e = inode_create(parent_inode, basename.str, basename.len, kind, ret);
    
    if (e < 0) return e;
    
    return SUCCESS;
}

int vfs_mount(const fs_t *fs, const path_t *path)
{
    inode_t *mountpoint = NULL;
    int e = vfs_lookup(path, &mountpoint);
    if (e < 0) return e;
    
    e = fs_mount(fs, &mountpoint);
    if (e < 0) return e;
    return SUCCESS;
}

void vfs_init(void)
{
    inode_cache = cache_create("inode cache", sizeof(inode_t));
}

