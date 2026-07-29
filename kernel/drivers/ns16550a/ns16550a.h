#pragma once

#include <stdint.h>
#include <stdbool.h>

// Minimal bring-up from a compatible node anywhere in the FDT: just enough to
// make ns16550a_putc() work, for use as an early console before the HAL is up.
bool ns16550a_probe_fdt(const void* fdt);

// Full bring-up of one specific node, including IRQ registration. Requires the
// HAL to be initialized.
void ns16550a_init(const void* fdt, int node);

void ns16550a_putc(char c);
uint32_t ns16550a_irq();
void ns16550a_irq_handler();