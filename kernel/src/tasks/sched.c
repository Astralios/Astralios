#include <stddef.h>
#include <stdint.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/cpu/cpu.h>
#include <arch/x86_64/mm/paging.h>
#endif

#include <misc/debug.h>

#include <tasks/sched.h>

#define STACK_PAGES_NUM 4

extern void context_switch(task_t *curr, task_t *next);
extern page_table_t *root_pt;

size_t next_pid = 0;

list_t task_list = { .next = &task_list, .prev = &task_list };
task_t *curr_task = NULL;
task_t *idle_task = NULL;

void task_schedule(task_t *task)
{
    list_insert(&task->list, &task_list);
}

void task_unschedule(task_t *task)
{
    // TODO: Clean up
    list_remove(&task->list);
}

extern void task_prelude(void);
static void task_postlude(void)
{
    task_unschedule(curr_task);   
    yield();
}

static void idle_loop()
{
    sti();
    hcf();
}

// FIXME: Use the actual kmalloc function
task_t* kernel_task_create(void (*entry)()) {
    task_t *task = NULL;
    uint64_t *stack = NULL;
    uint64_t *rsp = (uint64_t*)((char*)stack + STACK_PAGES_NUM * PAGE_SIZE);

    *(--rsp) = (uint64_t)task_postlude;
    *(--rsp) = (uint64_t)entry;
    *(--rsp) = (uint64_t)task_prelude;
    rsp -= 6;

    task->pid = next_pid++;
    task->rsp = (uint64_t)rsp;
    list_init(&task->list);

    return task;
}

static task_t kernel_task = {0};

void scheduler_init(void)
{
    idle_task = kernel_task_create(idle_loop);
    curr_task = &kernel_task;
}

task_t *scheduler_select_task(void)
{
    if (list_empty(&task_list)) return idle_task;
   
    task_t *next = (task_t*)task_list.next;
    list_remove(task_list.next);
    list_insert(&next->list, &task_list); 
    return next;
}

// TODO: Swap page tables for user tasks; Set tss rsp0;
void _context_switch(task_t *from, task_t *to)
{
    (void)from;
    info(_context_switch, "switching to pid: %zu", to->pid);

    curr_task = to;
}

void yield(void)
{
    cli();
    scheduler_switch();
    sti();
}

void scheduler_switch(void)
{
    task_t *new_task = scheduler_select_task();
    if (curr_task != new_task) context_switch(curr_task, new_task);
}

