#pragma once

#include <kernel.h>
#include <stdint.h>
#include <libds/include/list.h>

typedef struct 
{
    void*  sp;
    void*  base;
    size_t size;
} stack_t;

typedef struct
{ 
    list_t  list;
    stack_t kernel_stack;
    stack_t user_stack;
    size_t  pid;
    page_table_t *pt;
} task_t;

void sched_init(void);
void sched_switch(void);
void sched_schedule(task_t *task);
void sched_unschedule(task_t *task);

task_t* kernel_task_create(void (*entry)());
task_t* user_task_create(uint64_t addr, size_t size);

void    task_sleep(void);
void    task_wake_all_up(void);
void    task_yield(void);

