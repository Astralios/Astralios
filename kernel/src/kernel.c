#define SMLIB_IMPLEMENTATION
#include "smlib.h"

#include "arch/x86_64/gdt.h"
#include "arch/x86_64/interrupts/controller/pic.h"
#include "arch/x86_64/interrupts/idt.h"
#include "arch/x86_64/mm/paging.h"
#include "arch/x86_64/pit.h"
#include "devices/tty.h"
#include "devices/serial.h"

#include "drivers/fb.h"

#include "drivers/ps2/ps2.h"
#include "drivers/ps2/ps2_keyboard.h"
#include "fs/tar.h"

#include "fs/tmpfs.h"
#include "misc/debug.h"
#include "fs/vfs.h"

#include "bootstub.h"
#include "mm/kheap.h"
#include "mm/pmm.h"

#ifdef __ARCH_X86_64__
#include "arch/x86_64/def.h"
#include "arch/x86_64/tss.h"
#include "arch/x86_64/cpu.h"
#endif

#include "tasks/sched.h"
#include "string.h"
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>

extern vaddr_t hhdm_end;
kernel_params_t *kernel_params = NULL;
tty_t tty;
fb_driver_t *fb_driver0 = NULL;
page_table_t *root_pt = NULL;

void kpaging_init(page_table_t **root_pt)
{
    uint64_t root_pt_paddr = pmm_alloc(1);
    *root_pt = paddr_ptr(root_pt_paddr);
    memset((*root_pt)->entries, 0, PAGE_SIZE);
    map_kernel_to_pt(*root_pt);
    map_memmap_to_pt(*root_pt);
    write_cr3(root_pt_paddr);
}

extern void enable_sse(void);

float sqrt_sse(float x)
{
    float result;
    __asm__(
        "sqrtss %1, %0"
        : "=x"(result)
        : "x"(x));
    return result;
}

void task_A(void)
{
    while (1)
    {
        cli();
        srdebug(task_A, "Task A is running!");
        sti();
        yield();
    }
}

void task_B(void)
{
    while (1)
    {
        cli();
        srdebug(task_B, "Task B is running!");
        sti();
        yield();
    }
}

// Taken from: https://github.com/purpleK2/kernel/blob/e6ff7cdeb50169a6715c6e6c0919a46f6f7b4245/src/arch/x86_64/math/fpu.c#L7
void init_fpu()
{
    // TODO: Check if FPU is available
    asm volatile(
        "mov %%cr0, %%rax\n"
        "and $~(1 << 2), %%rax\n" // clear EM: disable emulation
        "or  $(1 << 1), %%rax\n"  // set MP: monitor FPU
        "or  $(1 << 5), %%rax\n"  // set NE: enable internal x87 error reporting
        "mov %%rax, %%cr0\n" ::
            : "rax");

    asm volatile("fninit");
}

static inline float fcosf(float x)
{
    float result;
    __asm__ __volatile__(
        "flds %1\n\t" // fpu stack shit
        "fcos\n\t"
        "fstps %0\n\t" // pop the float
        : "=m"(result)
        : "m"(x));
    return result;
}

static inline float fsinf(float x)
{
    float result;
    __asm__ __volatile__(
        "flds %1\n\t" // fpu stack shit
        "fsin\n\t"
        "fstps %0\n\t" // pop the float
        : "=m"(result)
        : "m"(x));
    return result;
}

void kmain(kernel_params_t *params)
{
    kernel_params = params;
    serial_init();
    gdt_init();
    idt_init();
    tss_init();
    pmm_init();

    kpaging_init(&root_pt);
    kheap_init(root_pt, hhdm_end, (32 * 1024 * 1024 * 1024l) / PAGE_SIZE);

    fb_driver0 = fb_driver_init(&kernel_params->fbs[0]);
    fb_driver_fill_entirely(fb_driver0, BLUE);

    kheap_debug();
    pic_init();

    ps2_init();
    ps2_keyboard_init();

    fs_mount(&tmpfs, &vfs_root);

    path_t initrd = vfs_path_from_abs("/initrd");
    inode_t *initrd_inode = NULL;
    vfs_create(&initrd, INODE_DIR, &initrd_inode);

    tar_extract(kernel_params->modules.modules[1].addr);

    if (!initrd_inode)
        debug("no initrd");
    path_t hellotxt = vfs_path_from_abs("/initrd/astralis-welcome.txt");
    inode_t *hellotxt_inode = NULL;
    vfs_lookup(&hellotxt, &hellotxt_inode);
    if (!hellotxt_inode)
        debug("non-existant");
    char read_buf[256] = {0};
    inode_read(hellotxt_inode, read_buf, 32, 0);

    memset(read_buf, 0, 256);
    path_t bye = vfs_path_from_abs("/initrd/astralis-bye.txt");
    inode_t *bye_inode = NULL;
    vfs_lookup(&bye, &bye_inode);
    inode_read(bye_inode, read_buf, 16, 0);

    enable_sse();
    init_fpu();

    float val = fsinf(PI);
    srdebug(kmain, "val: %d", (int)(val * 1000));

    scheduler_init();
    task_schedule(kernel_task_create(task_A));
    task_schedule(kernel_task_create(task_B));

    // pit_init(100);

    hcf();
}
