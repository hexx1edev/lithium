#include "fdt_probe.h"

#include <stddef.h>
#include <libfdt.h>
#include <printf.h>

#include <drivers/registry.h>
#include <kernel/boot_info.h>

static const driver_t* driver_for_node(const void* fdt, int node) {
    for (size_t i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        if (drivers[i].device.type == FDT_DEVICE && (fdt_node_check_compatible(fdt, node, drivers[i].device.fdt_compatible) == 0))
            return &drivers[i];
    }

    return NULL;
}

void drivers_fdt_probe() {
    if (!info || !info->fdt) {
        printf("[probe] no FDT available, probe failed\n");
        return;
    }

    const void* fdt = info->fdt;
    unsigned matched = 0;

    for (int node = fdt_next_node(fdt, -1, NULL); node >= 0; node = fdt_next_node(fdt, node, NULL)) {
        const driver_t* driver = driver_for_node(fdt, node);
        if (!driver)
            continue;

        const char* name = fdt_get_name(fdt, node, NULL);
        printf("[probe] %s: matched driver \"%s\"\n", name ? name : "<unnamed>", driver->device.fdt_compatible);

        driver->fdt_init((void*)fdt, node);
        matched++;
    }

    printf("[probe] driver probe finished, %u driver(s) initialized\n", matched);
}