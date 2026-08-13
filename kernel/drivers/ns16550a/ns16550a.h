#pragma once

#include <stdint.h>
#include <stdbool.h>

bool ns16550a_init_console_fdt(void* fdt, int node);
bool ns16550a_init_fdt();

void ns16550a_putc(char c);
void ns16550a_irq_handler();
void ns16550a_remap();