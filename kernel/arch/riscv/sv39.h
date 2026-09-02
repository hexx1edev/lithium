#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef uint64_t pte_t;
typedef pte_t* pagetable_t;

#define PTE_V (1UL << 0)  // Valid
#define PTE_R (1UL << 1)  // Read
#define PTE_W (1UL << 2)  // Write
#define PTE_X (1UL << 3)  // Execute
#define PTE_U (1UL << 4)  // User
#define PTE_G (1UL << 5)  // Global
#define PTE_A (1UL << 6)  // Accessed
#define PTE_D (1UL << 7)  // Dirty

#define PAGE_SHIFT  12

// PPN is at [53:10] in PTE
#define PA2PTE(pa)  ((((uint64_t)(pa)) >> PAGE_SHIFT) << 10)
#define PTE2PA(pte) (((pte) >> 10) << PAGE_SHIFT)

void sv39_init();
bool sv39_map(uint64_t virtual, uint64_t physical, uint64_t size, uint64_t permissions);
void sv39_unmap(uint64_t virtual, uint64_t size);
void sv39_enable(int extra_caller_frames);
uint64_t sv39_virt_to_phys(uint64_t virtual);