#include <hal/hal.h>
#include <hal/interrupts.h>
#include <hal/irq.h>
#include <hal/exceptions.h>
#include <hal/mmu.h>
#include <memory/pmm/pmm.h>
#include <kernel/memory.h>

#include <stdbool.h>

#include <libfdt.h>
#include <printf.h>

#include "csr.h"
#include "plic.h"
#include "trap.h"
#include "sv39.h"

static bool plic_present = false;

extern char _stack_top[];

void hal_init() {
    printf("[riscv] riscv hart %lu\n", (unsigned long)info->arch.riscv_hartid);

    trap_init();
    printf("[riscv] traps initialized\n");

    if (!info->fdt || fdt_check_header(info->fdt) != 0) {
        printf("[riscv] no usable FDT, interrupt controller unavailable\n");
        return;
    }

    plic_present = plic_probe(info->fdt);
    if (!plic_present)
        printf("[riscv] no PLIC found, IRQs will not be delivered\n");
}

void hal_irq_register_handler(uint32_t irq, hal_irq_handler_t handler) {
    if (plic_present)
        plic_register_handler(irq, handler);
}

void hal_irq_enable(uint32_t irq, uint32_t priority) {
    if (plic_present)
        plic_enable_irq(irq, priority);
}

void hal_enable_interrupts() {
    csr_set_sstatus(CSR_SSTATUS_SIE);
}

void hal_wait_for_interrupt() {
    asm volatile("wfi");
}

void hal_set_memory_exception_handler(hal_memory_exception_handler handler) {
    trap_set_memory_exception_handler(handler);
}

_Noreturn void hal_halt() {
    for (;;)
        hal_wait_for_interrupt();
}

void hal_reserve_memory() {
    printf("[riscv] reserving OpenSBI memory region\n");
    pmm_reserve(0x80000000, 0x200000);
}

void hal_mmu_init() {
    printf("[riscv] initializing Sv39 MMU\n");
    sv39_init();
}

bool hal_mmu_map(uint64_t virtual, uint64_t physical, uint64_t size, hal_mmu_perm_t permissions) {
    uint64_t perms = 0;
    switch (permissions) {
        case PERM_KERNEL_CODE:
            perms = (PTE_R | PTE_X);
            break;
        case PERM_KERNEL_DATA:
            perms = (PTE_R | PTE_W);
            break;
        case PERM_KERNEL_RODATA:
            perms = PTE_R;
            break;
        case PERM_MMIO:
            perms = (PTE_R | PTE_W);
            break;
        case PERM_USER_CODE:
            perms = (PTE_R | PTE_X | PTE_U);
            break;
        case PERM_USER_DATA:
            perms = (PTE_R | PTE_W | PTE_U);
            break;
    }

    return sv39_map(virtual, physical, size, perms);
}

// Walks the frame-pointer chain (ra at -8(fp), caller's fp at -16(fp))
// starting at `frame`, adding `offset` to `frames` saved return addresses.
// For use right before switching address spaces mid-execution: frames
// already on the stack have saved return addresses pointing at code that's
// about to become unreachable, so patch them in place so a later `ret`
// through them still lands somewhere valid.
void hal_fixup_call_frames(void* frame, int frames, uint64_t offset) {
    uint64_t* fp = (uint64_t*)frame;
    for (int i = 0; i < frames && fp; i++) {
        fp[-1] += offset;         // saved ra
        fp = (uint64_t*)fp[-2];   // caller's saved fp
    }
}

void hal_mmu_enable(int extra_caller_frames) {
    printf("[riscv] enabling Sv39 MMU\n");
    sv39_enable(extra_caller_frames);
}

void hal_update_stack() {
    uint64_t stack_top = (uint64_t)_stack_top;

    asm volatile("mv sp, %0" :: "r"(PA2VA(stack_top))); 
}