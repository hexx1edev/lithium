#include "probe.h"
#include "console.h"

#include <stddef.h>
#include <libfdt.h>
#include <printf.h>

#include <drivers/registry.h>
#include <kernel/boot_info.h>

static const console_driver_t* console_driver_for_node(const void* fdt, int node) {
    for (size_t i = 0; i < sizeof(console_drivers) / sizeof(console_drivers[0]); i++) {
        if (console_drivers[i].device.type == FDT_DEVICE && (fdt_node_check_compatible(fdt, node, console_drivers[i].device.fdt_compatible) == 0))
            return &console_drivers[i];
    }

    return NULL;
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
        console_register(driver->putc);
        printf_set_callback(console_putc);

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