#ifdef __ARCH_X86_64__
#include <arch/x86_64/cpu/cpu.h>
#include <arch/x86_64/desc/gdt.h>
#include <arch/x86_64/mm/paging.h>
#endif

#include <misc/debug.h>
#include <misc/helpers.h>

#include <mm/vheap.h>

#include <libs/libds/list.h>

#include <stdbool.h>
#include <stdint.h>

#include <tasks/sched.h>

#define TASK_STACK_NUM_PAGES 4

extern void switch_to(task_t *from, task_t *to);

static bool scheduler_ready = false;
static size_t next_pid = 0;

static list_t running_queue     = LIST_HEAD_INIT(running_queue);
static list_t reaper_queue      = LIST_HEAD_INIT(reaper_queue);
static list_t reaper_wait_queue = LIST_HEAD_INIT(reaper_wait_queue);
static list_t waiting_queue     = LIST_HEAD_INIT(waiting_queue);

static task_t *curr_task   = NULL;
static task_t *idle_task   = NULL;
static task_t *reaper_task = NULL;

static task_t dummy_task = {0};

static inline void task_queue_add(list_t *queue, task_t *task)
{
    list_insert(queue, &task->list);
}

static inline void task_queue_remove(task_t *task)
{
    list_remove(&task->list);
}

void task_switch(task_t *from, task_t *to)
{
    (void)from;
    curr_task = to;

    if (to->pt && to->pt != from->pt)
        pt_swap(to->pt);
}

void sched_schedule(task_t *task)
{
    task_queue_add(&running_queue, task);
}

void sched_unschedule(task_t *task)
{
    task_queue_remove(task);
}

task_t *sched_select_task(void)
{
    if (list_empty(&running_queue)) 
        return idle_task;
    task_t *next = (task_t*)running_queue.next;
    sched_unschedule(next);
    sched_schedule(next);
    return next;
}

void sched_switch(void)
{
    if (!scheduler_ready) 
        return;

    task_t *new_task = sched_select_task();
    if (curr_task != new_task)
    {
        debug("Switching to: %x", curr_task->pid);
        switch_to(curr_task, new_task);
    }
}

void task_yield(void)
{
    cli();
    sched_switch();
    sti();
}

void task_sleep(void)
{
    cli();
    sched_unschedule(curr_task);
    task_queue_add(&waiting_queue, curr_task);
    sti();
}

void task_wake_all_up(void)
{
    cli();

    list_t *node = waiting_queue.next;
    while (node != &waiting_queue)
    {
        list_t *next = node->next;

        task_t *task = (task_t*)node;
        task_queue_remove(task);
        sched_schedule(task);

        node = next;
    }
    sti();
}

static inline task_t* task_alloc(void)
{
    return vmalloc(sizeof(task_t));
}

static inline void stack_allocate(stack_t *stack)
{
    stack->base = vmalloc(TASK_STACK_NUM_PAGES * PAGE_SIZE);
    stack->size = TASK_STACK_NUM_PAGES * PAGE_SIZE;
    stack->sp = (char*)stack->base + stack->size;
}

static void task_prelude(void)
{
    sti();
}

static void task_postlude(void)
{
    cli();
    sched_unschedule(curr_task);
    task_queue_add(&reaper_queue, curr_task);
    while (!list_empty(&reaper_wait_queue))
    {
        task_t *reaper = container_of(reaper_wait_queue.next, task_t, list);
        sched_schedule(reaper);
    }

    sched_switch();
}

task_t *kernel_task_create(void (*entry)())
{
    task_t *task = task_alloc();
    stack_allocate(&task->kernel_stack);
    
    uint64_t *sp = (uint64_t*)task->kernel_stack.sp;
    *(--sp) = (uint64_t)task_postlude;
    *(--sp) = (uint64_t)entry;
    *(--sp) = (uint64_t)task_prelude;
    sp -= 6;

    task->pt = NULL;
    task->kernel_stack.sp = sp;
    task->pid = next_pid++;
    list_init(&task->list);

    return task;
}

extern void user_task_prelude(void);

task_t *user_task_create(void (*entry)())
{
    task_t *task = task_alloc();
    stack_allocate(&task->user_stack);
    stack_allocate(&task->kernel_stack);

    uint64_t *krsp = (uint64_t*)task->kernel_stack.sp;
    
    *(--krsp) = offsetof(gdt_t, user_data_segment) | 0x3;
    *(--krsp) = (uint64_t)task->user_stack.sp;
    *(--krsp) = 0x202;
    *(--krsp) = offsetof(gdt_t, user_code_segment) | 0x3;
    *(--krsp) = (uint64_t)entry;
    *(--krsp) = (uint64_t)user_task_prelude;
    krsp -= 6;

    task->pt = pt_create();
    pt_join_kernel(task->pt, krnlctx(pt));

    task->pid = next_pid++;
    list_init(&task->list);

    return task;
}

static void idle_loop(void)
{
    sti();
    debug("Idling");
    hcf();
}

static void reaper_task_entry(void)
{   
    cli();
    while (!list_empty(&reaper_queue))
    {
        task_t *task = (task_t*)reaper_queue.next;
        task_queue_remove(task); 
        vfree(task->kernel_stack.base);
        vfree(task);
    }

    sched_unschedule(curr_task);
    sti();
    task_yield();
}

void sched_init(void)
{
    scheduler_ready = true;
    idle_task = kernel_task_create(idle_loop); 
    reaper_task = kernel_task_create(reaper_task_entry);
    curr_task = &dummy_task;
}

