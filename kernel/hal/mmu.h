#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    PERM_KERNEL_CODE,
    PERM_KERNEL_DATA,
    PERM_KERNEL_RODATA,
    PERM_MMIO,
    PERM_USER_CODE,
    PERM_USER_DATA
} hal_mmu_perm_t;

void hal_mmu_init();
bool hal_mmu_map(uint64_t virtual, uint64_t physical, uint64_t size, hal_mmu_perm_t permissions);

void hal_fixup_call_frames(void* frame, int frames, uint64_t offset);
void hal_mmu_enable(int extra_caller_frames);