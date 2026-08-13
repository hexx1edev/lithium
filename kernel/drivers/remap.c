#include "remap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <drivers/registry.h>
#include <kernel/memory.h>

void drivers_map_memory() {
    for (size_t i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        drivers[i].device.fdt_compatible = (const char*)(uintptr_t)PA2VA((uint64_t)drivers[i].device.fdt_compatible);
        drivers[i].fdt_init = (bool (*)(void*, int))(uintptr_t)PA2VA((uint64_t)drivers[i].fdt_init);
        drivers[i].map_memory = (void (*)(void))(uintptr_t)PA2VA((uint64_t)drivers[i].map_memory);

        drivers[i].map_memory();
    }
}