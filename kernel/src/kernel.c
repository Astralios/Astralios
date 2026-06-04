#include "arch/x86_64/cpu/fpu.h"
#include "arch/x86_64/cpu/sse.h"
#include "fs/tar.h"
#include "fs/tmpfs.h"
#include "fs/vfs.h"
#include "libs/libds/include/ringbuf.h"
#include "libs/libinput/include/kbd.h"
#include "mm/vheap.h"
#include "modules.h"
#include "tasks/sched.h"
#include "terminal/fbtty.h"
#include <kernel.h>

#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdatomic.h>

#include <bootstub.h>

#include <misc/debug.h>

#include <mm/pmm/pmm.h>
#include <mm/vmm/vmm.h>
#include <mm/slab.h>

#include <devs/serial.h>

#include <drvs/ps2/ps2.h>
#include <drvs/ps2/kbd/ps2_kbd.h>
#include <drvs/ps2/ps2_mouse.h>

#include <gfx/surface.h>

#include <firmware/acpi/acpi.h>

#include <misc/arrlen.h>

#ifdef __ARCH_X86_64__
#include <arch/x86_64/mm/paging.h>
#include <arch/x86_64/cpu/cpu.h>
#include <arch/x86_64/desc/gdt.h>
#include <arch/x86_64/desc/idt.h>
#include <arch/x86_64/ints/pic.h>
#include <arch/x86_64/ints/apic.h>
#endif

krnl_ctx_t krnl_ctx;

extern vaddr_t hhdm_end;
vmm_t vmm;
fbtty_t fbtty;

void task_a_entry()
{
    static int count = 0;
    while (count != 20)
    {
        cli();
        count++;
        debug("Task A");
    
        if (count % 2 == 0) 
        {
            task_wake_all_up();
        }

        sti();
        yield();
    }
}



void task_b_entry()
{
    while (1)
    {
        cli();
        debug("Task B");
        sti();
        task_sleep();
        yield();
    }
}


// typedef struct __attribute__((packed)) {
//     char header[2];
//     uint32_t file_size;
//     uint32_t reserved;
//     uint32_t data_offset;
// } bmp_header_t;

// typedef struct __attribute__((packed)) {
//     uint32_t header_size;
//     uint32_t width;
//     uint32_t height;
//     uint16_t color_planes;
//     uint16_t bits_per_pixel;
//     uint32_t compression;
//     uint32_t image_size;
//     uint32_t x_res;
//     uint32_t y_res;
//     uint32_t colors;
//     uint32_t important_colors;
// } bmp_dib_header_t;

// typedef struct __attribute__((packed)) {
//     uint8_t blue;
//     uint8_t green;
//     uint8_t red;
// } rgb_pixel_t;
int x = 0;
int y = 0;
bool left_click = false;

void kmain(bootloader_ctx_t *ctx)
{
    krnlctx(bootloader_ctx) = ctx;

    serial_init();
    gdt_init();
    idt_init();
    tss_init();    
    pmm_init();

    krnlctx(pt) = pt_create();    
    pt_map_kernel(krnlctx(pt));
    pt_map_memmap(krnlctx(pt));
    pt_swap(krnlctx(pt));
    
    acpi_init();

    pic_init();
    apic_init();

    vheap_init();
    ps2_init();
    ps2_keyboard_init();
    
    ps2_mouse_init();

    slab_init();
    vfs_init();
    fs_mount(&tmpfs, &vfs_root);
    
    path_t initrd = vfs_path_from_abs("/initrd");
    inode_t *initrd_inode = NULL;

    vfs_create(&initrd, INODE_DIR, &initrd_inode);
    if (!initrd_inode) 
        debug("No initrd");
    
    const module_t *module = module_find(&bootctx(modules), "/boot/initrd.tar");

    debug("%x", module);
    tar_extract(module->addr);
    path_t hello = vfs_path_from_abs("/initrd/welcome.txt");
    inode_t *hello_inode = NULL;
    vfs_lookup(&hello, &hello_inode);

    scheduler_init(); 
    task_t *task_a = task_create(task_a_entry);
    task_t *task_b = task_create(task_b_entry);
    scheduler_schedule(task_a);
    scheduler_schedule(task_b);

    hcf();
}

