export ARCH := riscv

export PROJECT_ROOT := $(dir $(abspath $(lastword $(MAKEFILE_LIST))))

include scripts/$(ARCH).mk

export BUILD_DIR := $(PROJECT_ROOT)build

export LIBK_DIR := $(PROJECT_ROOT)libk
export LIBFDT_DIR := $(PROJECT_ROOT)libfdt
export KERNEL_DIR := $(PROJECT_ROOT)kernel

export CC := clang
export LD := clang
export AR := llvm-ar
export OBJCOPY := llvm-objcopy
export STRIP := llvm-strip

export CFLAGS := \
    $(ARCH_CFLAGS) \
    -nostdlib \
    -ffreestanding \
    -fno-builtin \
    -I$(LIBK_DIR) \
    -I$(LIBFDT_DIR) \
    -Wall -Wextra -Werror   # better to use these flags sometimes

export LDFLAGS := \
    $(ARCH_LDFLAGS) \
    -nostdlib \
    -fno-builtin