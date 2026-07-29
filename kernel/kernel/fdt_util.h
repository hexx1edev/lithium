#pragma once

#include <stdint.h>
#include <stdbool.h>

bool fdt_get_reg(const void* fdt, int node, uint64_t* addr, uint64_t* size);
bool fdt_get_irq(const void* fdt, int node, uint32_t* irq);
