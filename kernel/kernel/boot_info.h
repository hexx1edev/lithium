#pragma once

#include <stdint.h>

typedef struct {
    uint64_t riscv_hartid;
} arch_boot_info;

typedef struct {
    arch_boot_info arch;
    const void* fdt;
} boot_info;

extern boot_info* info;

static inline boot_info* kernel_get_boot_info() {
    return info;
}
