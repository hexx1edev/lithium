#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <libfdt.h>

bool fdt_get_reg(const void* fdt, int node, uint64_t* addr, uint64_t* size);
bool fdt_get_irq(const void* fdt, int node, uint32_t* irq);
uint64_t fdt_read_cells(const fdt32_t* cells, int count);