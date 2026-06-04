#include "arch/x86_64/cpu/fpu.h"
#include "arch/x86_64/cpu/sse.h"
#include "fs/tar.h"
#include "fs/tmpfs.h"
#include "fs/vfs.h"
#include "libs/libds/include/ringbuf.h"
#include "libs/libinput/include/kbd.h"
#include "mm/vheap.h"
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

page_table_t *root_pt = NULL;
extern vaddr_t hhdm_end;
vmm_t vmm;
fbtty_t fbtty;

void kpaging_init(page_table_t **root_pt)
{
    uint64_t root_pt_paddr = pmm_palloc(1);
    *root_pt = paddr_ptr(root_pt_paddr);
    memset((*root_pt)->entries, 0, PAGE_SIZE);
    map_kernel_to_pt(*root_pt);
    map_memmap_to_pt(*root_pt);
    write_cr3(root_pt_paddr);
    info(paging_init, "Initalized!");
}
  

extern ringbuf_t key_ringbuf;

char key_to_char(key_t key, bool shift) {
    switch (key) {
        case KEY_1: return shift ? '!' : '1';
        case KEY_2: return shift ? '@' : '2';
        case KEY_3: return shift ? '#' : '3';
        case KEY_4: return shift ? '$' : '4';
        case KEY_5: return shift ? '%' : '5';
        case KEY_6: return shift ? '^' : '6';
        case KEY_7: return shift ? '&' : '7';
        case KEY_8: return shift ? '*' : '8';
        case KEY_9: return shift ? '(' : '9';
        case KEY_0: return shift ? ')' : '0';

        case KEY_A: return shift ? 'A' : 'a';
        case KEY_B: return shift ? 'B' : 'b';
        case KEY_C: return shift ? 'C' : 'c';
        case KEY_D: return shift ? 'D' : 'd';
        case KEY_E: return shift ? 'E' : 'e';
        case KEY_F: return shift ? 'F' : 'f';
        case KEY_G: return shift ? 'G' : 'g';
        case KEY_H: return shift ? 'H' : 'h';
        case KEY_I: return shift ? 'I' : 'i';
        case KEY_J: return shift ? 'J' : 'j';
        case KEY_K: return shift ? 'K' : 'k';
        case KEY_L: return shift ? 'L' : 'l';
        case KEY_M: return shift ? 'M' : 'm';
        case KEY_N: return shift ? 'N' : 'n';
        case KEY_O: return shift ? 'O' : 'o';
        case KEY_P: return shift ? 'P' : 'p';
        case KEY_Q: return shift ? 'Q' : 'q';
        case KEY_R: return shift ? 'R' : 'r';
        case KEY_S: return shift ? 'S' : 's';
        case KEY_T: return shift ? 'T' : 't';
        case KEY_U: return shift ? 'U' : 'u';
        case KEY_V: return shift ? 'V' : 'v';
        case KEY_W: return shift ? 'W' : 'w';
        case KEY_X: return shift ? 'X' : 'x';
        case KEY_Y: return shift ? 'Y' : 'y';
        case KEY_Z: return shift ? 'Z' : 'z';

        case KEY_SPACE: return ' ';
        case KEY_MINUS: return shift ? '_' : '-';
        case KEY_EQUALS: return shift ? '+' : '=';
        case KEY_OPEN_BRACKET: return shift ? '{' : '[';
        case KEY_CLOSED_BRACKET: return shift ? '}' : ']';
        case KEY_SEMICOLON: return shift ? ':' : ';';
        case KEY_SINGLE_QUOTE: return shift ? '"' : '\'';
        case KEY_BACK_TICK: return shift ? '~' : '`';
        case KEY_BACKSLASH: return shift ? '|' : '\\';
        case KEY_COMMA: return shift ? '<' : ',';
        case KEY_PERIOD: return shift ? '>' : '.';
        case KEY_SLASH: return shift ? '?' : '/';

        case KEY_KEYPAD_0: return '0';
        case KEY_KEYPAD_1: return '1';
        case KEY_KEYPAD_2: return '2';
        case KEY_KEYPAD_3: return '3';
        case KEY_KEYPAD_4: return '4';
        case KEY_KEYPAD_5: return '5';
        case KEY_KEYPAD_6: return '6';
        case KEY_KEYPAD_7: return '7';
        case KEY_KEYPAD_8: return '8';
        case KEY_KEYPAD_9: return '9';
        case KEY_KEYPAD_PLUS: return '+';
        case KEY_KEYPAD_MINUS: return '-';
        case KEY_KEYPAD_ASTERISK: return '*';
        case KEY_KEYPAD_SLASH: return '/';
        case KEY_KEYPAD_PERIOD: return '.';

        default: return ' ';
    }
}



#define SMLIB_IMPLEMENTATION
#include <libs/smlib.h>
typedef struct model3d_t
{
    const vec3_t *vcs;
    size_t num_faces;
    const int (*fs)[3];
} model3d_t;

vec2_t to_surface_coordinate(gfx_surface_t *surface, vec2_t p)
{
    return (vec2_t){
        .x = (p.x + 1) / 2 * surface_height(surface),
        .y = (-p.y + 1) / 2 * surface_height(surface),
    };
}

void draw_line(gfx_surface_t *surface, vec2_t p1, vec2_t p2)
{
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;

    int steps = (int)(abs(dx) > abs(dy) ? abs(dx) : abs(dy));

    if (steps == 0)
    {
        gfx_surface_put_pixel(surface, p1.x, p1.y, 0xFF);
        return;
    }
    
    float x_inc = dx / steps;
    float y_inc = dy / steps;

    float x = p1.x;
    float y = p1.y;

    for (int i = 0; i <= steps; i++)
    {
        gfx_surface_put_pixel(surface, (size_t)x, (size_t)y, 0xFFFF00); 
        x += x_inc;
        y += y_inc;
    }
}

void clear(gfx_surface_t *surface, uint32_t color)
{
    for (size_t i = 0; i < surface_height(surface); ++i)
    {
        for (size_t j = 0; j < surface_width(surface); ++j)
        {
            gfx_surface_put_pixel(surface, j, i, color);
        }
    }
}

static float angle = 0;
vec3_t camera = vec3(0, 0, 0);
void model3d_render(gfx_surface_t *surface, model3d_t *model)
{
    for (size_t i = 0; i < model->num_faces; ++i)
    {
        const int *f = model->fs[i];

        for (size_t j = 0; j < 3; ++j)
        {
            vec3_t p1 = model->vcs[f[j]];
            vec3_t p2 = model->vcs[f[(j + 1) % 3]];

    
            p1.x -= camera.x;
            p1.y -= camera.y;
            p1.z -= camera.z;

            p2.x -= camera.x;
            p2.y -= camera.y;
            p2.z -= camera.z;

            vec2_t rp1 = to_surface_coordinate(
                    surface, 
                    vec3_project_to_vec2(p1)
                    );

            vec2_t rp2 = to_surface_coordinate(
                    surface, 
                     vec3_project_to_vec2(
                            p2
                        )
       
                    );

            draw_line(surface, rp1, rp2);
        }
    }
}

void redner()
{
    gfx_surface_t *surface = gfx_surface_create(&krnl_ctx.bootloader_ctx->fbs[0]);
    
    const int FPS = 60;
    const int fs[][3] = {
        {0, 1, 4},
        {1, 2, 4},
        {2, 3, 4},
        {3, 0, 4},
        {0, 2, 1},
        {0, 3, 2},
    };

    const vec3_t vcs[] = {
        vec3(0, 0, 0),
        vec3(0, 0, 1),
        vec3(1, 0, 1),
        vec3(1, 0, 0),
        vec3(0.5, 1, 0.5),
    };

    model3d_t model = {
        .vcs = vcs,
        .num_faces = 6,
        .fs = fs
    };

    const float dt = 1.0 / FPS;
    while (1)
    {
        kbd_ev_t ev;
        ringbuf_read(&key_ringbuf, &ev);
        if (ev.action == KEY_ACTION_PRESS)
        {
            switch (ev.keycode) {
                case KEY_W:
                {
                    camera.z += 0.1f;
                    break;
                }
                case KEY_S:
                {
                    camera.z -= 0.1f;
                    break;
                }
                case KEY_A:
                {
                    camera.x -= 0.1f;
                    break;
                }
                case KEY_D:
                {
                    camera.x += 0.1f;
                    break;
                }
                case KEY_SPACE:
                {
                    camera.y += 0.01f;
                    break;
                }
                case KEY_LEFT_SHIFT:
                {
                    camera.y -= 0.01f;
                    break;
                }
                default: {}
            }
        }

        angle += dt * HALF_PI;
        clear(surface, 0);
        model3d_render(surface, &model);
        gfx_surface_sync(surface, 0, surface_height(surface) * surface_width(surface) - 1);
        
    }
}

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


int x = 50;
int y = 50;
bool left_click = false;

void kmain(bootloader_ctx_t *ctx)
{
    krnl_ctx.bootloader_ctx = ctx;
    serial_init();
    gdt_init();
    idt_init();
    tss_init();    
    pmm_init();
    kpaging_init(&root_pt);
    krnl_ctx.pt = root_pt;
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
    if (!initrd_inode) debug("No initrd");
    tar_extract(krnl_ctx.bootloader_ctx->modules.modules[1].addr);
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

