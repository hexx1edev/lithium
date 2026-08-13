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

// Patches the saved return address of `frames` enclosing C call frames,
// walking the frame-pointer chain starting at `frame` (as returned by
// __builtin_frame_address(0) in the innermost frame), by adding `offset`
// to each one. For use right before switching address spaces mid-
// execution, so frames already on the stack - whose saved return
// addresses point at code that's about to become unreachable - can still
// `ret` correctly afterwards.
void hal_fixup_call_frames(void* frame, int frames, uint64_t offset);

// Enables the MMU. Also fixes up the saved return addresses of
// `extra_caller_frames` enclosing C stack frames above hal_mmu_enable's
// own caller, so they can return normally too once translation is active.
// Pass 0 if only your immediate caller needs to return normally.
void hal_mmu_enable(int extra_caller_frames);