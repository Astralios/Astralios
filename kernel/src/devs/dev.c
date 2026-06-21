#include <libs/libds/list.h>
#include <mm/slab.h>
#include <misc/err.h>
#include <devs/dev.h>
#include <string.h>

static cache_t *dev_cache = NULL;
static list_t   dev_list = LIST_HEAD_INIT(dev_list);

void devs_init(void)
{
    dev_cache = cache_create("dev_cache", sizeof(dev_t));
}

ssize_t dev_write(dev_t *device, const void *buf, size_t count)
{
    return device->write ? device->write(buf, count) : -UNIMPLEMENTED;
}

ssize_t dev_read(dev_t *device, void *buf, size_t count)
{
    return device->read ? device->read(buf, count) : -UNIMPLEMENTED;
}

dev_t *dev_create(const char *name, int major, int minor, write_fn_t write, read_fn_t read)
{
    dev_t *device = (dev_t*)cache_alloc(dev_cache);
    device->name = name;
    device->write = write;
    device->read = read;
    device->major = major;
    device->minor = minor;
    
    list_append(&dev_list, &device->list);

    return device;
}

dev_t* dev_find(const char *name)
{
    foreach(node, &dev_list)
    {
        dev_t *dev = (dev_t*)node;
        if (strcmp(dev->name, name) == 0)
            return dev;
    }
    return NULL;
}



