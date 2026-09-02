export ARCH := riscv

export PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include scripts/$(ARCH).mk

export BUILD_DIR := $(PROJECT_ROOT)build

export LIBK_DIR := $(PROJECT_ROOT)libk
export LIBFDT_DIR := $(PROJECT_ROOT)libfdt
export KERNEL_DIR := $(PROJECT_ROOT)kernel

export RAMDISK_DIR := $(PROJECT_ROOT)ramdisk

export CFLAGS := \
    $(ARCH_CFLAGS) \
    -nostdlib \
    -ffreestanding \
    -fno-builtin \
    -fno-jump-tables \
    -I$(LIBK_DIR) \
    -I$(LIBFDT_DIR) \
    -Wall -Wextra       # better to use these flags sometimes

export LDFLAGS := \
    $(ARCH_LDFLAGS) \
    -nostdlib \
    -fno-builtin