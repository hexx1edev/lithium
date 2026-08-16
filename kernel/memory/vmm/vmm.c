#include "vmm.h"

bool vmm_map(uint64_t virtual, uint64_t physical, uint64_t size, hal_mmu_perm_t permissions) {
    return hal_mmu_map(virtual, physical, size, permissions);
}