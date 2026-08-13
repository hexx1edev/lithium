#pragma once

#if defined(__riscv) && (__riscv_xlen == 64)
    #define ARCH_RISCV64        1
    #define ARCH_VA_BITS        39          // Sv39
    #define ARCH_CANONICAL_BIT  (ARCH_VA_BITS - 1)  // bit 38
#else
    #error "Unsupported architecture"
#endif