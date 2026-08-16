#include "sv39.h"
#include <kernel/panic.h>
#include <kernel/memory.h>
#include <memory/pmm/pmm.h>
#include <memory.h>
#include <hal/mmu.h>
#include <printf.h>

#define VPN_MASK 0x1FFUL

#define VPN(va, level) \
    (((uint64_t)(va) >> (PAGE_SHIFT + 9 * (level))) & VPN_MASK)

#define SATP_MODE_SV39 (8UL << 60)

static inline uint64_t make_satp(pte_t *root) {
    return SATP_MODE_SV39 | (((uint64_t)root) >> PAGE_SHIFT);
}

static pagetable_t root;

static bool mmu_enabled = false;

static inline pte_t* table_ptr(uint64_t pa) {
    return (pte_t*)(mmu_enabled ? PA2VA(pa) : pa);
}

static pte_t* walk(uint64_t virtual, int alloc) {
    pte_t* table = table_ptr((uint64_t)root);

    for (int level = 2; level > 0; level--) {
        pte_t* pte = &table[VPN(virtual, level)];

        if (*pte & PTE_V) {
            table = table_ptr(PTE2PA(*pte));
        } else {
            if (!alloc) return NULL;

            void* new_table = pmm_alloc_page();
            if (!new_table) return NULL;

            pte_t* new_table_ptr = table_ptr((uint64_t)new_table);
            memset(new_table_ptr, 0, PAGE_SIZE);

            *pte = PA2PTE((uint64_t)new_table) | PTE_V; // 0 R/W/X -> non-leaf, pointer to next level
            table = new_table_ptr;
        }
    }

    return &table[VPN(virtual, 0)]; // PTE leaf level
}

void sv39_init() {
    root = pmm_alloc_page();

    if (!root) panic("[sv39] failed to allocate page for root table!\n");
    memset(root, 0, 4096);
}

bool sv39_map(uint64_t virtual, uint64_t physical, uint64_t size, uint64_t permissions) {
    if (size == 0) return false;

    uint64_t va_start = virtual & ~(PAGE_SIZE - 1);
    uint64_t va_end = (virtual + size - 1) & ~(PAGE_SIZE - 1);
    uint64_t pa_start = physical & ~(PAGE_SIZE - 1);

    for (uint64_t a = va_start, p = pa_start; ; a += PAGE_SIZE, p += PAGE_SIZE) {
        pte_t* pte = walk(a, 1);
        if (!pte) return false;
        if (*pte & PTE_V) return false; // already mapped - error!

        *pte = PA2PTE(p) | permissions | PTE_V;

        if (a == va_end) break;
    }
    return true;
}

extern void sv39_switch(uint64_t satp_value, uint64_t va_offset);

void sv39_enable(int extra_caller_frames) {
    // sv39_enable's own frame + hal_mmu_enable's frame are always fixed
    // extra_caller_frames covers however many more the caller wants
    hal_fixup_call_frames(__builtin_frame_address(0), 2 + extra_caller_frames, KERNEL_VA_HIGHHALF_BASE);

    mmu_enabled = true;

    sv39_switch(make_satp(root), KERNEL_VA_HIGHHALF_BASE);
}