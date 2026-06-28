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

typedef struct modules_t modules_t;
typedef struct modules_t
{
    void (*get_module)(modules_t *mods, module_t *mod, size_t i);
    uint64_t  num_modules;
} modules_t;

// TODO:  Maybe you don't even need modules
static inline void module_find(modules_t *modules, module_t *module, const char *name)
{
    for (size_t i = 0; i < modules->num_modules; ++i)
    {
        module_t mod;
        modules->get_module(modules, &mod, i);
        if (strcmp(mod.path, name) == 0)
        {
            *module = mod;
            return;
        }
    }
}


