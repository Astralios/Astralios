#pragma once
#include <stdint.h>
#include <stddef.h>
#include <string.h>

typedef struct
{
    void*    addr;
    uint64_t size;
    char*    path;
} module_t;

typedef struct
{
    uint64_t  num_modules;
    module_t* modules;
} modules_t;

// TODO:  Maybe you don't even need modules
static inline const module_t *module_find(const modules_t *modules, const char *name)
{
    for (size_t i = 0; i < modules->num_modules; ++i)
    {
        module_t *mod = &modules->modules[i];
        if (strcmp(mod->path, name) == 0) return mod;
    }
    return NULL;
}


