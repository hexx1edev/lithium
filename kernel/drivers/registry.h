#pragma once

#include <drivers/types.h>
#include <drivers/ns16550a/ns16550a.h>

static const driver_t drivers[] = {
    { {FDT_DEVICE, "ns16550a"}, ns16550a_init_fdt }
};

static const console_driver_t console_drivers[] = {
    { {FDT_DEVICE, "ns16550a"}, ns16550a_init_console_fdt, ns16550a_putc }
};