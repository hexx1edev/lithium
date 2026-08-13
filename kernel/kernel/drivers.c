#include "drivers.h"
#include <printf.h>
#include <drivers/fdt_probe.h>

void kernel_init_drivers() {
    printf("[kernel] probing drivers\n");
    drivers_fdt_probe();
}