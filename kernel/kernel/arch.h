#pragma once

#if defined(__riscv) && (__riscv_xlen == 64)
    #define KERNEL_VA_HIGHHALF_BASE 0xffffffc000000000
#else
    #error "Unsupported architecture"
#endif