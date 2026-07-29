#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef void (*plic_irq_handler_t)();

bool plic_probe(const void* fdt);

void plic_enable_irq(uint32_t irq, uint32_t priority);
void plic_register_handler(uint32_t irq, plic_irq_handler_t handler);
void plic_handle_interrupt();
