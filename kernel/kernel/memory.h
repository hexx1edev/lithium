#pragma once

#include <kernel/arch.h>

#define KERNEL_VA_HIGHHALF_BASE \
    (~((1ULL << ARCH_CANONICAL_BIT) - 1))

#define PA2VA(pa) (KERNEL_VA_HIGHHALF_BASE + (pa))

void kernel_init_memory();