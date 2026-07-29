#pragma once

#include <stdint.h>

typedef void (*hal_irq_handler_t)();

void hal_irq_register_handler(uint32_t irq, hal_irq_handler_t handler);
void hal_irq_enable(uint32_t irq, uint32_t priority);