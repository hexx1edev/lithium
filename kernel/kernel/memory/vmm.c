#include "vmm.h"
#include "hal/mmu.h"
#include <kernel/memory/pmm.h>
#include <util/math.h>
#include <printf.h>


bool vmm_map(uint64_t virtual, uint64_t physical, uint64_t size, hal_mmu_perm_t permissions) {
    return hal_mmu_map(virtual, physical, size, permissions);
}

void vmm_unmap(uint64_t virtual, uint64_t size) {
    hal_mmu_unmap(virtual, size);
}

uint64_t vmm_virt_to_phys(uint64_t virtual) {
    return hal_mmu_virt_to_phys(virtual);
}