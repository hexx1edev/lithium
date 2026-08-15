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

// info's address is materialized via a PC-relative auipc/addi pair. A caller
// that touches `info` both before and after a mid-function satp switch can
// have that materialization folded into one stack slot, computed once while
// PC was still physical, then reused after translation is live - a dangling
// pointer. noinline forces a fresh materialization in its own function, so
// calling this after the switch always resolves through the current PC.
__attribute__((noinline))
static inline boot_info* kernel_get_boot_info() {
    return info;
}
