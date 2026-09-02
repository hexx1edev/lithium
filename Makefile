include config.mk

.PHONY: all libk libfdt kernel init ramdisk clean

all: kernel ramdisk

libk:
	@echo "--> Building $@"
	@$(MAKE) -C libk

libfdt:
	@echo "--> Building $@"
	@$(MAKE) -C libfdt

kernel: libk libfdt
	@echo "--> Building $@"
	@$(MAKE) -C kernel

init:
	@echo "--> Building $@"
	@$(MAKE) -C init

ramdisk: init
	@echo "--> Creating ramdisk"
	@mkdir -p $(BUILD_DIR)/ramdisk
	@cp -r $(RAMDISK_DIR)/. $(BUILD_DIR)/ramdisk/
	@cd $(BUILD_DIR)/ramdisk && find . -print0 | cpio --null -o -H newc > $(BUILD_DIR)/rd.img

clean:
	@echo "--> Cleaning up"
	@rm -rf $(BUILD_DIR)