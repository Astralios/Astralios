#include "drvs/ps2/kbd/misc.h"
#include "libs/libinput/kbd.h"
#include <libs/libinput/mse.h>
#include <devs/dev.h>
#include <fs/tar.h>
#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <mm/vheap.h>
#include <modules.h>
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
    slab_init();

    devs_init();

    ps2_init();
    ps2_keyboard_init();
    
    ps2_mouse_init();

    vfs_init();
    fs_mount(&tmpfs, &vfs_root);

    path_t initrd = vfs_path_from_abs("/initrd");
    inode_t *initrd_inode = NULL;

    vfs_create(&initrd, INODE_DIR, &initrd_inode);
    if (!initrd_inode) 
        debug("No initrd");
    
    const module_t *module = module_find(&bootctx(modules), "/boot/initrd.tar");

    tar_extract(module->addr);
    path_t hello = vfs_path_from_abs("/initrd/welcome.txt");
    inode_t *hello_inode = NULL;
    vfs_lookup(&hello, &hello_inode);

    gfx_surface_t *surface = gfx_surface_create(&bootctx(fbs)[0]);
    dev_t *dev = dev_find("ps2-mse");
    int x = 50;
    int y = 50;
    while (1)
    {
        mse_ev_t ev = {0};
        if (dev_read(dev, &ev, 1) > 0)
        {
            gfx_draw_rect(surface, x, y, 25, 25, 0x0);
            gfx_surface_sync_chunk(surface, x, y, 25, 25);
            x += ev.dx;
            y += -ev.dy;
            gfx_draw_rect(surface, x, y, 25, 25, 0xFFFF00);
            gfx_surface_sync_chunk(surface, x, y, 25, 25);
        }
    }

    hcf();
}

