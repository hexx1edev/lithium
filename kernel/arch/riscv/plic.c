#include "plic.h"

#include <libfdt.h>
#include <printf.h>

#include <hal/mmu.h>
#include <kernel/fdt_util.h>
#include <kernel/memory.h>

// SiFive layout
#define PRIORITY_OFFSET 0x0
#define ENABLE_OFFSET 0x2000
#define ENABLE_CTX_STRIDE 0x80
#define CONTEXT_OFFSET 0x200000
#define CONTEXT_CTX_STRIDE 0x1000

#define MAX_IRQ 96

// temporary
#define CONTEXT_HART0_S 1

static volatile uint8_t* base;
static plic_irq_handler_t handlers[MAX_IRQ];

static inline volatile uint32_t* priority_reg(uint32_t irq) {
    return (volatile uint32_t*)(base + PRIORITY_OFFSET + irq * 4);
}

static inline volatile uint32_t* enable_reg(uint32_t ctx, uint32_t irq) {
    return (volatile uint32_t*)(base + ENABLE_OFFSET + ctx * ENABLE_CTX_STRIDE + (irq / 32) * 4);
}

static inline volatile uint32_t* threshold_reg(uint32_t ctx) {
    return (volatile uint32_t*)(base + CONTEXT_OFFSET + ctx * CONTEXT_CTX_STRIDE);
}

static inline volatile uint32_t* claim_reg(uint32_t ctx) {
    return (volatile uint32_t*)(base + CONTEXT_OFFSET + ctx * CONTEXT_CTX_STRIDE + 4);
}

bool plic_probe(const void* fdt) {
    int node = fdt_node_offset_by_compatible(fdt, -1, "sifive,plic-1.0.0");
    if (node < 0)
        return false;

    uint64_t addr, size;
    if (!fdt_get_reg(fdt, node, &addr, &size))
        return false;

    hal_mmu_map(PA2VA(addr), addr, size, PERM_MMIO);
    base = (volatile uint8_t*)(uintptr_t)PA2VA(addr);

    *threshold_reg(CONTEXT_HART0_S) = 0;

    printf("[plic] base=0x%lx context=%u\n", (unsigned long)addr, CONTEXT_HART0_S);

    return true;
}

void plic_enable_irq(uint32_t irq, uint32_t priority) {
    *priority_reg(irq) = priority;

    volatile uint32_t* reg = enable_reg(CONTEXT_HART0_S, irq);
    *reg |= (1U << (irq % 32));

    printf("[plic] irq %u enabled at priority %u\n", irq, priority);
}

void plic_register_handler(uint32_t irq, plic_irq_handler_t handler) {
    if (irq < MAX_IRQ)
        handlers[irq] = handler;
}

void plic_handle_interrupt() {
    volatile uint32_t* claim = claim_reg(CONTEXT_HART0_S);
    uint32_t irq = *claim;

    if (irq == 0)
        return;

    if (irq < MAX_IRQ && handlers[irq] != NULL)
        handlers[irq]();
    else
        printf("plic: unhandled irq %u\n", irq);

    *claim = irq;
}
