include make.conf

.PHONY: directories initrd all clean

all: directories initrd

directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(ISO_ROOT_DIR)
	@mkdir -p $(BOOT_DIR)
	@mkdir -p $(OBJS_DIR)

INITRD_DIR  := initrd/
INITRD_ZIP  := initrd.tar

initrd: directories
	@tar -cvf $(BOOT_DIR)/$(INITRD_ZIP) $(INITRD_DIR)

run:
	@qemu-system-x86_64 \
	-serial stdio \
	-d int \
	-D qemu.log \
	--no-reboot \
	--no-shutdown \
	-m 2g \
	$(BUILD_DIR)/image.iso

include build.mk

all: iso

clean:
	rm -rf $(BUILD_DIR)

iso: 
	make -C target/$(ARCH)/$(BOOTLDR)/
