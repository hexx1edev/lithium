#include "ns16550a.h"

#include <libfdt.h>
#include <printf.h>

#include <hal/irq.h>
#include <hal/mmu.h>
#include <kernel/fdt_util.h>
#include <kernel/memory.h>
#include <memory/vmm/vmm.h>

#define REG_THR 0 // Transmit Holding Register (write) / Receiver Buffer (read)
#define REG_IER 1 // Interrupt Enable Register
#define REG_LSR 5 // Line Status Register

#define LSR_DATA_READY 0x01
#define IER_RX_AVAILABLE 0x01

#define IRQ_PRIORITY 1

static volatile uint8_t* base;
static uint64_t mmio_size;
static uint32_t irq;

bool ns16550a_init_console_fdt(void* fdt, int node) {
    uint64_t addr, size;
    if (!fdt_get_reg(fdt, node, &addr, &size)) {
        printf("[ns16550a] node has no usable reg\n");
        return false;
    }

    if (!fdt_get_irq(fdt, node, &irq)) {
        printf("[ns16550a] node has no interrupts property\n");
        return false;
    }

    base = (volatile uint8_t*)(uintptr_t)addr;
    mmio_size = size;
    printf("[ns16550a] base=0x%lx irq=%u\n", (unsigned long)addr, irq);

    return true;
}

bool ns16550a_init_fdt() {
    // we assume that console probe was done

    hal_irq_register_handler(irq, ns16550a_irq_handler);
    hal_irq_enable(irq, IRQ_PRIORITY);

    base[REG_IER] = IER_RX_AVAILABLE;

    printf("[ns16550a] rx interrupt enabled at irq %d\n", irq);

    return true;
}

void ns16550a_putc(char c) {
    if (!base) return;

    if (c == '\n')
        base[REG_THR] = '\r';

    base[REG_THR] = c;
}

void ns16550a_irq_handler() {
    if (base[REG_LSR] & LSR_DATA_READY) {
        ns16550a_putc((char)base[REG_THR]);
    }
}

void ns16550a_remap() {
    uint64_t phys = (uint64_t)(uintptr_t)base;

    vmm_map(phys, PA2VA(phys), mmio_size, PERM_MMIO);
    base = (volatile uint8_t*)(uintptr_t)PA2VA(phys);

    // re-register with new address
    printf_set_callback(ns16550a_putc);
}
