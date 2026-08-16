#pragma once

#include <stdint.h>

typedef struct {
    uint64_t riscv_hartid;
} arch_boot_info;

typedef struct {
    arch_boot_info arch;
    const void* fdt;
    const void* ramdisk_start;
    const void* ramdisk_end;
    const uint64_t load_addr;
} boot_info;

extern boot_info* info;

static inline boot_info* kernel_get_boot_info() {
    return info;
}
