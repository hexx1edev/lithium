#include "fdt_util.h"

#include <libfdt.h>

static uint64_t fdt_read_cells(const fdt32_t* cells, int n) {
    uint64_t value = 0;

    for (int i = 0; i < n; i++)
        value = (value << 32) | fdt32_to_cpu(cells[i]);

    return value;
}

bool fdt_get_reg(const void* fdt, int node, uint64_t* addr, uint64_t* size) {
    int parent = fdt_parent_offset(fdt, node);
    if (parent < 0)
        return false;

    int addr_cells = fdt_address_cells(fdt, parent);
    int size_cells = fdt_size_cells(fdt, parent);
    if (addr_cells < 0 || size_cells < 0)
        return false;

    int len;
    const fdt32_t* reg = fdt_getprop(fdt, node, "reg", &len);
    if (!reg || len < (int)((addr_cells + size_cells) * sizeof(fdt32_t)))
        return false;

    *addr = fdt_read_cells(reg, addr_cells);
    *size = fdt_read_cells(reg + addr_cells, size_cells);

    return true;
}

bool fdt_get_irq(const void* fdt, int node, uint32_t* irq) {
    int len;
    const fdt32_t* prop = fdt_getprop(fdt, node, "interrupts", &len);
    if (!prop || len < (int)sizeof(fdt32_t))
        return false;

    *irq = fdt32_to_cpu(prop[0]);

    return true;
}
