#include "fdt_probe.h"

#include <stddef.h>
#include <libfdt.h>
#include <printf.h>

#include <drivers/registry.h>
#include <kernel/boot_info.h>

// drivers_map_memory() already rewrote drivers[]'s pointer fields to their
// high-half aliases before kernel_init_drivers() runs, so nothing here
// needs to translate anything itself.
static const driver_t* driver_for_node(const void* fdt, int node) {
    for (size_t i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        if (drivers[i].device.type == FDT_DEVICE && (fdt_node_check_compatible(fdt, node, drivers[i].device.fdt_compatible) == 0))
            return &drivers[i];
    }

    return NULL;
}

static const console_driver_t* console_driver_for_node(const void* fdt, int node) {
    for (size_t i = 0; i < sizeof(console_drivers) / sizeof(console_drivers[0]); i++) {
        if (console_drivers[i].device.type == FDT_DEVICE && (fdt_node_check_compatible(fdt, node, console_drivers[i].device.fdt_compatible) == 0))
            return &console_drivers[i];
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

bool drivers_console_fdt_probe() {
    if (!info || !info->fdt) {
        printf("[probe] no FDT available, console probe failed\n");
        return false;
    }

    const void* fdt = info->fdt;

    const console_driver_t* driver = NULL;
    bool found = false;

    for (int node = fdt_next_node(fdt, -1, NULL); node >= 0; node = fdt_next_node(fdt, node, NULL)) {
        driver = console_driver_for_node(fdt, node);
        if (!driver)
            continue;

        const char* name = fdt_get_name(fdt, node, NULL);
        printf("[probe] %s: matched console driver \"%s\"\n", name ? name : "<unnamed>", driver->device.fdt_compatible);

        driver->fdt_console_init((void*)fdt, node);
        printf_set_callback(driver->putc);

        found = true;
        break;
    }

    if (found) {
        printf("[probe] console driver probe finished, found %s console driver\n", driver->device.fdt_compatible);
        return true;
    } else {
        printf("[probe] console driver probe finished, no console driver found\n");
        return false;
    }
}