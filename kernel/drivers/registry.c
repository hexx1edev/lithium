#include "registry.h"

driver_t drivers[DRIVERS_COUNT] = {
    { {FDT_DEVICE, "ns16550a"}, ns16550a_init_fdt, ns16550a_remap }
};

const console_driver_t console_drivers[CONSOLE_DRIVERS_COUNT] = {
    { {FDT_DEVICE, "ns16550a"}, ns16550a_init_console_fdt, ns16550a_putc }
};
