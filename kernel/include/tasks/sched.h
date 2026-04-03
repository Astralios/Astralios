#pragma once

#include "arch/x86_64/def.h"
#include "vendor/list.h"
#include <stdint.h>

#define TASK_NAME_MAX_LEN   64

typedef struct task_t task_t;
typedef struct task_t { 
    list_t list;
    vaddr_t rsp;
    size_t pid;
} task_t;

void yield(void);
void scheduler_init(void);
void scheduler_switch(void);

task_t* kernel_task_create(void (*entry)());
void task_schedule(task_t *task);
void task_unschedule(task_t *task);

