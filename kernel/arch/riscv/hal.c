#include <hal/hal.h>
#include <hal/interrupts.h>
#include <hal/irq.h>
#include <hal/exceptions.h>
#include <memory/pmm/pmm.h>

#include <stdbool.h>

#include <libfdt.h>
#include <printf.h>

#include "csr.h"
#include "plic.h"
#include "trap.h"

static bool s_plic_present = false;

void hal_init() {
    printf("[riscv] riscv hart %lu\n", (unsigned long)info->arch.riscv_hartid);

    trap_init();
    printf("[riscv] traps initialized\n");

    if (!info->fdt || fdt_check_header(info->fdt) != 0) {
        printf("[riscv] no usable FDT, interrupt controller unavailable\n");
        return;
    }

    s_plic_present = plic_probe(info->fdt);
    if (!s_plic_present)
        printf("[riscv] no PLIC found, IRQs will not be delivered\n");
}

void hal_irq_register_handler(uint32_t irq, hal_irq_handler_t handler) {
    if (s_plic_present)
        plic_register_handler(irq, handler);
}

void hal_irq_enable(uint32_t irq, uint32_t priority) {
    if (s_plic_present)
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