#include "arch/x86_64/desc/tss.h"
#include "arch/x86_64/hw/io.h"
#include "arch/x86_64/pit.h"
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

#define EI_NIDENT 16
#define ELFMAG0 0x7f
#define ELFMAG1 'E'
#define ELFMAG2 'L'
#define ELFMAG3 'F'

typedef uint64_t Elf64_Addr;
typedef uint64_t Elf64_Off;
typedef uint16_t Elf64_Half;
typedef uint32_t Elf64_Word;
typedef int32_t Elf64_Sword;
typedef uint64_t Elf64_Xword;
typedef int64_t Elf64_Sxword;

typedef struct
{
    unsigned char e_ident[EI_NIDENT];
    Elf64_Half e_type;
    Elf64_Half e_machine;
    Elf64_Word e_version;
    Elf64_Addr e_entry;
    Elf64_Off e_phoff;
    Elf64_Off e_shoff;
    Elf64_Word e_flags;
    Elf64_Half e_ehsize;
    Elf64_Half e_phentsize;
    Elf64_Half e_phnum;
    Elf64_Half e_shentsize;
    Elf64_Half e_shnum;
    Elf64_Half e_shstrndx;
} Elf64_Ehdr;

typedef struct 
{
    Elf64_Word p_type;
    Elf64_Word p_flags;
    Elf64_Off p_offset;
    Elf64_Addr p_vaddr;
    Elf64_Addr p_paddr;
    Elf64_Xword p_filesz;
    Elf64_Xword p_memsz;
    Elf64_Xword p_align;
} Elf64_Phdr;

void user_test_entry()
{
    syscall_set_number(5);
    __asm__("int $0x80");
    while (1);
}

static void play_sound(uint32_t nFrequence) {
 	uint32_t Div;
 	uint8_t tmp;
 
 	Div = 1193180 / nFrequence;
 	outb(0x43, 0xb6);
 	outb(0x42, (uint8_t) (Div) );
 	outb(0x42, (uint8_t) (Div >> 8));
 
 	tmp = inb(0x61);
  	if (tmp != (tmp | 3)) {
 		outb(0x61, tmp | 3);
 	}
 }
 
 static void nosound() {
 	uint8_t tmp = inb(0x61) & 0xFC;
     
 	outb(0x61, tmp);
 }
 
 void beep() {
 	 play_sound(1000);
 	 pit_sleep_ms(10);
 	 nosound();
 }

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

    path_t elf_file_path = vfs_path_from_abs("/initrd/main");
    inode_t *elf_file = NULL;
    if (vfs_lookup(&elf_file_path, &elf_file) >= 0)
    {
        debug("Found the elf file successfully!");
    }

    paddr_t phys = pmm_alloc(1);

    pt_map(
        krnlctx(pt),

     phys,
     0x400000,   
     PAGE_FLAG_PRESENT |
            PAGE_FLAG_READ_WRITE |
            PAGE_FLAG_USER_SUPERVISOR
    );

    uint8_t *code = paddr_ptr(phys);
    code[0] = 0x48; 
    code[1] = 0xC7; 
    code[2] = 0xC0; 
    code[3] = 0x01; 
    code[4] = 0x00; 
    code[5] = 0x00; 
    code[6] = 0x00;
    code[7] = 0xCD;
    code[8] = 0x80;
    code[9] = 0xEB;
    code[10] = 0xFE;

    paddr_t stack = pmm_alloc(1);
 
    pt_map(
        krnlctx(pt),
        
     stack,
     0x510000, 
     PAGE_FLAG_PRESENT |
            PAGE_FLAG_READ_WRITE |
            PAGE_FLAG_USER_SUPERVISOR
    );   

    tss.rsp0 = read_sp();
    extern void userspace_jump();
    userspace_jump();

    hcf();
}
