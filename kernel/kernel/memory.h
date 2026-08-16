#pragma once

#include <kernel/arch.h>

#define PA2VA(pa) (KERNEL_VA_HIGHHALF_BASE + (pa))
#define VA2PA(va) ((va) - KERNEL_VA_HIGHHALF_BASE)

void kernel_init_memory();