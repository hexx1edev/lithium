#include "probe.h"

#include <stddef.h>

#include <libfdt.h>
#include <printf.h>

#include <drivers/ns16550a/ns16550a.h>
#include <kernel/boot_info.h>

typedef struct {
    const char* compatible;
    void (*init)(const void* fdt, int node);
} driver;

// every driver the kernel knows how to bring up
static const driver drivers[] = {
    { "ns16550a", ns16550a_init },
};

static const driver* driver_for_node(const void* fdt, int node) {
    for (size_t i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        if (fdt_node_check_compatible(fdt, node, drivers[i].compatible) == 0)
            return &drivers[i];
    }

    return NULL;
}

void drivers_probe() {
    if (!info || !info->fdt) {
        printf("[probe] no FDT available, probe failed\n");
        return;
    }

    const void* fdt = info->fdt;
    unsigned matched = 0;

    for (int node = fdt_next_node(fdt, -1, NULL); node >= 0; node = fdt_next_node(fdt, node, NULL)) {
        const driver* drv = driver_for_node(fdt, node);
        if (!drv)
            continue;

        const char* name = fdt_get_name(fdt, node, NULL);
        printf("[probe] %s: matched driver \"%s\"\n", name ? name : "<unnamed>", drv->compatible);

        drv->init(fdt, node);
        matched++;
    }

    printf("[probe] driver probe finished, %u driver(s) initialized\n", matched);
}