#include "ns16550a.h"

#include <libfdt.h>
#include <printf.h>

#include <hal/irq.h>
#include <kernel/fdt_util.h>

#define REG_THR 0 // Transmit Holding Register (write) / Receiver Buffer (read)
#define REG_IER 1 // Interrupt Enable Register
#define REG_LSR 5 // Line Status Register

#define LSR_DATA_READY 0x01
#define IER_RX_AVAILABLE 0x01

#define IRQ_PRIORITY 1

static volatile uint8_t* base;
static uint32_t irq;

bool ns16550a_probe_fdt(const void* fdt) {
    int node = fdt_node_offset_by_compatible(fdt, -1, "ns16550a");
    if (node < 0)
        return false;

    uint64_t addr, size;
    if (!fdt_get_reg(fdt, node, &addr, &size))
        return false;

    if (!fdt_get_irq(fdt, node, &irq))
        return false;

    base = (volatile uint8_t*)(uintptr_t)addr;

    return true;
}

void ns16550a_init(const void* fdt, int node) {
    uint64_t addr, size;
    if (!fdt_get_reg(fdt, node, &addr, &size)) {
        printf("[ns16550a] node has no usable reg, skipping\n");
        return;
    }

    if (!fdt_get_irq(fdt, node, &irq)) {
        printf("[ns16550a] node has no interrupts property, skipping\n");
        return;
    }

    base = (volatile uint8_t*)(uintptr_t)addr;
    printf("[ns16550a] base=0x%lx irq=%u\n", (unsigned long)addr, irq);

    hal_irq_register_handler(irq, ns16550a_irq_handler);
    hal_irq_enable(irq, IRQ_PRIORITY);

    base[REG_IER] = IER_RX_AVAILABLE;

    printf("[ns16550a] rx interrupt enabled\n");
}

void ns16550a_putc(char c) {
    if (c == '\n')
        base[REG_THR] = '\r';

    base[REG_THR] = c;
}

uint32_t ns16550a_irq() {
    return irq;
}

void ns16550a_irq_handler() {
    if (base[REG_LSR] & LSR_DATA_READY) {
        ns16550a_putc((char)base[REG_THR]);
    }
}
