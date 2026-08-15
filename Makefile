include config.mk

.PHONY: all libk libfdt kernel ramdisk clean

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

ramdisk:
	@echo "--> Creating ramdisk"
	@cd $(RAMDISK_DIR) && find . -print0 | cpio --null -o -H newc > $(PROJECT_ROOT)rd.img

clean:
	@echo "--> Cleaning up"
	@rm -rf $(BUILD_DIR)