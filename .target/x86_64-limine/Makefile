KERNEL_FOLDER   	:= kernel
SRC_FOLDER      	:= $(KERNEL_FOLDER)/src
BUILD_FOLDER    	:= .build
OBJS_FOLDER 		:= $(BUILD_FOLDER)/objs
LIBC_FOLDER     	:= libc
BIN_FOLDER      	:= bin
BOOTSTUB_FOLDER 	:= bootstub
MAKEFILE_DIR    	:= .target/x86_64-limine
LIMINE_FOLDER   	:= $(MAKEFILE_DIR)/limine
ISO_ROOT        	:= $(BUILD_FOLDER)/iso_root

KERNEL_BIN        	:= astralisos
LIMINE_BOOTSTUB     := $(BOOTSTUB_FOLDER)/limine/limine_bootstub.c
LIMINE_BOOTSTUB_OBJ := $(OBJS_FOLDER)/$(BOOTSTUB_FOLDER)/limine/limine_bootstub.c.o
ISO 				:= $(BUILD_FOLDER)/image.iso

CPPFLAGS 			:= -I$(LIMINE_FOLDER) -I$(BOOTSTUB_FOLDER) -I$(LIBC_FOLDER) -DLIMINE_API_REVISION=4
CFLAGS    			:= -Wall -Wextra -std=gnu11 -ffreestanding -fno-stack-protector \
             		   -fno-stack-check -fno-PIC -m64 -march=x86-64 -mno-80387 \
             		   -mno-mmx -mno-sse -mno-sse2 -mno-red-zone -mcmodel=kernel -g
LDFLAGS 			:= -Wl,-m,elf_x86_64 -Wl,--build-id=none -nostdlib -static \
        			   -z max-page-size=0x1000 -T $(MAKEFILE_DIR)/linker.ld

KERNEL 				:= $(BUILD_FOLDER)/$(BIN_FOLDER)/$(KERNEL_BIN)

C_SOURCES   		:= $(shell find $(SRC_FOLDER) $(LIBC_FOLDER) external/ -name '*.c')
ASM_SOURCES 		:= $(shell find $(SRC_FOLDER) -name '*.asm')

C_OBJS   			:= $(patsubst %.c, $(OBJS_FOLDER)/%.c.o, $(C_SOURCES))
ASM_OBJS 			:= $(patsubst %.asm, $(OBJS_FOLDER)/%.asm.o, $(ASM_SOURCES))

ALL_OBJS 			:= $(C_OBJS) $(ASM_OBJS) $(LIMINE_BOOTSTUB_OBJ)

COLOR_RESET := \033[0m
COLOR_GREEN := \033[32m
COLOR_BLUE := \033[34m
COLOR_YELLOW := \033[33m
COLOR_PURPLE := \033[35m
FONT_BOLD := \033[1m

.PHONY: all directories

directories:
	@mkdir -p $(ISO_ROOT)/boot/limine
	@mkdir -p $(ISO_ROOT)/EFI/BOOT
	@for dir in $(sort $(dir $(ALL_OBJS))); do mkdir -p $$dir; done

all: $(ISO)
	@echo "$(COLOR_BLUE)[MAKE]$(COLOR_RESET) Building Limine bootloader..."
	@$(MAKE) -C $(MAKEFILE_DIR)/limine

$(LIMINE_BOOTSTUB_OBJ): $(LIMINE_BOOTSTUB) | directories
	@echo "$(COLOR_GREEN)[CC]$(COLOR_RESET) $< $(COLOR_YELLOW)(Limine bootstub)$(COLOR_RESET)"
	@cc $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(KERNEL): $(ALL_OBJS)
	@echo "$(COLOR_GREEN)[LD]$(COLOR_RESET) $@"
	@cc $(CFLAGS) $(LDFLAGS) $(ALL_OBJS) -o $@

$(ISO): $(KERNEL) $(BUILD_FOLDER)
	@echo "$(COLOR_BLUE)[ISO]$(COLOR_RESET) Creating ISO with Limine bootloader..."
	@mkdir -p $(ISO_ROOT)/boot/limine $(ISO_ROOT)/EFI/BOOT
	@cp $(KERNEL) $(ISO_ROOT)/boot/$(KERNEL_BIN)
	@cp -v $(MAKEFILE_DIR)/limine.conf $(ISO_ROOT)/boot/limine/
	@cp -v $(MAKEFILE_DIR)/limine/limine-bios.sys \
	       $(MAKEFILE_DIR)/limine/limine-bios-cd.bin \
	       $(MAKEFILE_DIR)/limine/limine-uefi-cd.bin \
	       $(ISO_ROOT)/boot/limine/
	@cp -v $(MAKEFILE_DIR)/limine/BOOTX64.EFI \
	       $(MAKEFILE_DIR)/limine/BOOTIA32.EFI \
	       $(ISO_ROOT)/EFI/BOOT/
	@xorriso -as mkisofs -R -r -J \
	         -b boot/limine/limine-bios-cd.bin \
	         -no-emul-boot -boot-load-size 4 -boot-info-table \
	         -hfsplus -apm-block-size 2048 \
	         --efi-boot boot/limine/limine-uefi-cd.bin \
	         -efi-boot-part --efi-boot-image \
	         --protective-msdos-label \
	         $(ISO_ROOT) -o $(ISO)
	@$(MAKEFILE_DIR)/limine/limine bios-install $(ISO)
	@echo "$(COLOR_GREEN)[DONE]$(COLOR_RESET) Limine ISO created: $(ISO)"
