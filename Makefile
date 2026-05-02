BOOTLOADER   	?= limine
CURR_DIR		:= ./
KERNEL_FOLDER 	:= kernel
SRC_FOLDER   	:= kernel/src
BUILD_FOLDER 	:= .build
BIN_FOLDER   	:= bin
BIN_NAME     	:= astralisos
LIBC_FOLDER  	:= libc
EXTERNAL_FOLDER	:= external
BOOTSTUB_FOLDER := bootstub
ISO_ROOT     	:= $(BUILD_FOLDER)/iso_root
OBJS_FOLDER  	:= $(BUILD_FOLDER)/objs

CFLAGS    		:= -Wall -Wextra -std=gnu11 -ffreestanding -fno-stack-protector \
             		-fno-stack-check -fno-PIC -m64 -march=x86-64 \
             		-mno-mmx -mno-red-zone -mcmodel=kernel -g \
					-mno-80387 -msse 
CPPFLAGS  		:= -I$(BOOTSTUB_FOLDER) -I$(KERNEL_FOLDER)/include -I$(LIBC_FOLDER) -I$(EXTERNAL_FOLDER) -D __ARCH_X86_64__ -D __DEBUG__ -MMD -MP
NASMFLAGS 		:= -Wall -f elf64

C_SOURCES    	:= $(shell find $(SRC_FOLDER) $(LIBC_FOLDER) $(EXTERNAL_FOLDER) -name '*.c')
ASM_SOURCES  	:= $(shell find $(SRC_FOLDER) -name '*.asm')

#
C_OBJS   			:= $(patsubst %.c, $(OBJS_FOLDER)/%.c.o, $(C_SOURCES))
ASM_OBJS 			:= $(patsubst %.asm, $(OBJS_FOLDER)/%.asm.o, $(ASM_SOURCES))
ALL_OBJS := $(C_OBJS) $(ASM_OBJS)

DEPS := $(C_OBJS:.o=.d)

KERNEL := $(BUILD_FOLDER)/$(BIN_FOLDER)/$(BIN_NAME)

ISO := $(BUILD_FOLDER)/image.iso

COLOR_RESET := \033[0m
COLOR_GREEN := \033[32m
COLOR_BLUE := \033[34m
COLOR_YELLOW := \033[33m
COLOR_PURPLE := \033[35m
FONT_BOLD := \033[1m

.PHONY: all clean run iso directories

all: $(ALL_OBJS) 
	make -f ./.target/x86_64-limine/Makefile all

directories:
	@mkdir -p $(BUILD_FOLDER)
	@mkdir -p $(BUILD_FOLDER)/$(BIN_FOLDER)
	@mkdir -p $(OBJS_FOLDER)
	@mkdir -p $(ISO_ROOT)/boot/limine
	@mkdir -p $(ISO_ROOT)/EFI/BOOT
	@for dir in $(sort $(dir $(ALL_OBJS))); do mkdir -p $$dir; done

$(OBJS_FOLDER)/%.c.o: %.c | directories
	@echo "$(COLOR_GREEN)[CC]$(COLOR_RESET) $<"
	@cc $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(OBJS_FOLDER)/%.asm.o: %.asm | directories
	@echo "$(COLOR_PURPLE)[ASM]$(COLOR_RESET) $<"
	@nasm $(NASMFLAGS) $< -o $@

run:
	@echo "$(COLOR_BLUE)[QEMU]$(COLOR_RESET) Starting virtual machine..."
	@qemu-system-x86_64 \
		-serial stdio \
	    -d int \
	    -D $(BUILD_FOLDER)/qemu.log \
	    $(ISO) \
	    --no-reboot \
	    --no-shutdown \
		-m 2G

debug:
	@echo "$(COLOR_BLUE)[QEMU+GDB]$(COLOR_RESET) Starting with GDB server..."
	@qemu-system-x86_64 \
	    -serial stdio \
	    -d int \
	    -D $(BUILD_FOLDER)/qemu.log \
	    $(ISO) \
	    --no-reboot \
	    --no-shutdown \
	    -s -S

clean:
	@echo "$(COLOR_BLUE)[CLEAN]$(COLOR_RESET) Cleaning the build folder..."
	@rm -rf $(BUILD_FOLDER)

-include $(DEPS)

help:
	@echo "$(COLOR_BLUE)$(FONT_BOLD)AstralisOS$(COLOR_RESET) $(COLOR_BLUE)Build System$(COLOR_RESET)"
	@echo ""
	@echo "Commands:"
	@echo "  $(COLOR_YELLOW)all$(COLOR_RESET)       - Build the ISO image"
	@echo "  $(COLOR_YELLOW)run$(COLOR_RESET)       - Run ISO in QEMU"
	@echo "  $(COLOR_YELLOW)debug$(COLOR_RESET)     - Run ISO in QEMU with debugging"
	@echo "  $(COLOR_YELLOW)clean$(COLOR_RESET)     - Clean the Build folder"
	@echo "  $(COLOR_YELLOW)help$(COLOR_RESET)      - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  $(COLOR_YELLOW)BOOTLOADER$(COLOR_RESET) - Select bootloader (default: limine)"
