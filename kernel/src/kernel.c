#include "arch/x86_64/desc/tss.h"
#include "arch/x86_64/hw/io.h"
#include "arch/x86_64/pit.h"
#include <misc/panic.h>
#include "drvs/ps2/kbd/misc.h"
#include "drvs/rtc/misc.h"
#include "drvs/rtc/rtc.h"
#include "libinput/include/kbd.h"
#include <drvs/cmos.h>
#include <drvs/rtc/rtc.h>
#include "misc/helpers.h"
#include "tasks/sched.h"
#include "tasks/syscall.h"
#include <libinput/include/mse.h>
#include <devs/dev.h>
#include <fs/tar.h>
#include <fs/tmpfs.h>
#include <fs/vfs.h>
#include <mm/vheap.h>
#include <modules.h>
#include <kernel.h>
#include <fs/initrd.h>

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

#include <terminal/fbtty.h>

krnl_ctx_t krnl_ctx;
fbtty_t fbtty;

char key_to_char(key_t key, bool shift) {
  switch (key) {
  case KEY_1:
    return shift ? '!' : '1';
  case KEY_2:
    return shift ? '@' : '2';
  case KEY_3:
    return shift ? '#' : '3';
  case KEY_4:
    return shift ? '$' : '4';
  case KEY_5:
    return shift ? '%' : '5';
  case KEY_6:
    return shift ? '^' : '6';
  case KEY_7:
    return shift ? '&' : '7';
  case KEY_8:
    return shift ? '*' : '8';
  case KEY_9:
    return shift ? '(' : '9';
  case KEY_0:
    return shift ? ')' : '0';

  case KEY_A:
    return shift ? 'A' : 'a';
  case KEY_B:
    return shift ? 'B' : 'b';
  case KEY_C:
    return shift ? 'C' : 'c';
  case KEY_D:
    return shift ? 'D' : 'd';
  case KEY_E:
    return shift ? 'E' : 'e';
  case KEY_F:
    return shift ? 'F' : 'f';
  case KEY_G:
    return shift ? 'G' : 'g';
  case KEY_H:
    return shift ? 'H' : 'h';
  case KEY_I:
    return shift ? 'I' : 'i';
  case KEY_J:
    return shift ? 'J' : 'j';
  case KEY_K:
    return shift ? 'K' : 'k';
  case KEY_L:
    return shift ? 'L' : 'l';
  case KEY_M:
    return shift ? 'M' : 'm';
  case KEY_N:
    return shift ? 'N' : 'n';
  case KEY_O:
    return shift ? 'O' : 'o';
  case KEY_P:
    return shift ? 'P' : 'p';
  case KEY_Q:
    return shift ? 'Q' : 'q';
  case KEY_R:
    return shift ? 'R' : 'r';
  case KEY_S:
    return shift ? 'S' : 's';
  case KEY_T:
    return shift ? 'T' : 't';
  case KEY_U:
    return shift ? 'U' : 'u';
  case KEY_V:
    return shift ? 'V' : 'v';
  case KEY_W:
    return shift ? 'W' : 'w';
  case KEY_X:
    return shift ? 'X' : 'x';
  case KEY_Y:
    return shift ? 'Y' : 'y';
  case KEY_Z:
    return shift ? 'Z' : 'z';

  case KEY_SPACE:
    return ' ';
  case KEY_MINUS:
    return shift ? '_' : '-';
  case KEY_EQUALS:
    return shift ? '+' : '=';
  case KEY_OPEN_BRACKET:
    return shift ? '{' : '[';
  case KEY_CLOSED_BRACKET:
    return shift ? '}' : ']';
  case KEY_SEMICOLON:
    return shift ? ':' : ';';
  case KEY_SINGLE_QUOTE:
    return shift ? '"' : '\'';
  case KEY_BACK_TICK:
    return shift ? '~' : '`';
  case KEY_BACKSLASH:
    return shift ? '|' : '\\';
  case KEY_COMMA:
    return shift ? '<' : ',';
  case KEY_PERIOD:
    return shift ? '>' : '.';
  case KEY_SLASH:
    return shift ? '?' : '/';

  case KEY_KEYPAD_0:
    return '0';
  case KEY_KEYPAD_1:
    return '1';
  case KEY_KEYPAD_2:
    return '2';
  case KEY_KEYPAD_3:
    return '3';
  case KEY_KEYPAD_4:
    return '4';
  case KEY_KEYPAD_5:
    return '5';
  case KEY_KEYPAD_6:
    return '6';
  case KEY_KEYPAD_7:
    return '7';
  case KEY_KEYPAD_8:
    return '8';
  case KEY_KEYPAD_9:
    return '9';
  case KEY_KEYPAD_PLUS:
    return '+';
  case KEY_KEYPAD_MINUS:
    return '-';
  case KEY_KEYPAD_ASTERISK:
    return '*';
  case KEY_KEYPAD_SLASH:
    return '/';
  case KEY_KEYPAD_PERIOD:
    return '.';

  default:
    return ' ';
  }
}


void ps2_kbd_dev_task_entry(void)
{
    while (1)
    {
    dev_t *dev = dev_find("ps2-kbd");
    kbd_ev_t ev;
    if (dev_read(dev, &ev, sizeof(ev)) > 0)
    {
        if (ev.action == KEY_ACTION_PRESS)
        {
            if (ev.keycode == KEY_BACKSPACE)
            {
                fbtty_backspace(&fbtty);
            } else {
                char c = key_to_char(ev.keycode, false);
                fbtty_write_char(&fbtty, c);
            }
        }
    }
    }
}   

int x = 50;
int y = 50;

void ps2_mse_dev_task_entry(void)
{
    while (1)
    {
    dev_t *dev = dev_find("ps2-mse");
    mse_ev_t ev;
    if (dev_read(dev, &ev, sizeof(ev)) > 0)
    {   
        gfx_draw_rect(fbtty.surface, x, y, 10, 10, 0x0);
        gfx_surface_sync_chunk(fbtty.surface, x, y, 10, 10);
    
        x += ev.dx;
        y -= ev.dy;

        gfx_draw_rect(fbtty.surface, x, y, 10, 10, 0xFFFF00);
        gfx_surface_sync_chunk(fbtty.surface, x, y, 10, 10);
    }
    }
}   

void init_task_entry(void)
{
    task_t *ps2_kbd_dev = kernel_task_create(ps2_kbd_dev_task_entry);
    sched_schedule(ps2_kbd_dev);
    task_t *ps2_mse_dev = kernel_task_create(ps2_mse_dev_task_entry);
    sched_schedule(ps2_mse_dev);
}

#include <libds/include/hashmap.h>

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

    initrd_init();
    
    inode_t *welcome = NULL;
    initrd_get("welcome.txt", &welcome);
    char *buf[256] = {0};

    inode_read(welcome, buf, 30, 0);
    debug("%s", buf);

    fb_t fb;
    bootctx(fbs).get_fb(&bootctx(fbs), &fb, 0);

    fbtty_init(&fbtty, &fb);

    task_t *init_task = kernel_task_create(init_task_entry);
    sched_schedule(init_task);
    sched_init();

    hcf();
}
