include config.mk

.PHONY: all libk libfdt kernel clean

all: kernel

libk:
	@echo "--> Building $@"
	@$(MAKE) -C libk

libfdt:
	@echo "--> Building $@"
	@$(MAKE) -C libfdt

kernel: libk libfdt
	@echo "--> Building $@"
	@$(MAKE) -C kernel

clean:
	@echo "--> Cleaning up"
	@rm -rf $(BUILD_DIR)