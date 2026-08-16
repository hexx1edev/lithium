#include "remap.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <drivers/registry.h>
#include <kernel/memory.h>

void drivers_relocate(uint64_t delta) {
    if (delta == 0) return;

    for (size_t i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        drivers[i].device.fdt_compatible = (const char*)((uintptr_t)drivers[i].device.fdt_compatible + delta);
        drivers[i].fdt_init = (bool (*)(void*, int))((uintptr_t)drivers[i].fdt_init + delta);
        drivers[i].map_memory = (void (*)(void))((uintptr_t)drivers[i].map_memory + delta);
    }

    for (size_t i = 0; i < sizeof(console_drivers) / sizeof(console_drivers[0]); i++) {
        console_drivers[i].device.fdt_compatible = (const char*)((uintptr_t)console_drivers[i].device.fdt_compatible + delta);
        console_drivers[i].fdt_console_init = (bool (*)(void*, int))((uintptr_t)console_drivers[i].fdt_console_init + delta);
        console_drivers[i].putc = (void (*)(char))((uintptr_t)console_drivers[i].putc + delta);
    }
}

void drivers_map_memory() {
    for (size_t i = 0; i < sizeof(drivers) / sizeof(drivers[0]); i++) {
        drivers[i].device.fdt_compatible = (const char*)(uintptr_t)PA2VA((uint64_t)drivers[i].device.fdt_compatible);
        drivers[i].fdt_init = (bool (*)(void*, int))(uintptr_t)PA2VA((uint64_t)drivers[i].fdt_init);
        drivers[i].map_memory = (void (*)(void))(uintptr_t)PA2VA((uint64_t)drivers[i].map_memory);

        drivers[i].map_memory();
    }
}