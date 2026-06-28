#include <misc/panic.h>
#include <misc/debug.h>

__attribute__((noreturn))
void panic(const char *msg)
{
    debug("%s", msg);
    
    for (;;);
}

