#include <kernel/drivers.h>
#include <kernel/memory.h>
#include <drivers/fdt_probe.h>
#include <hal/hal.h>
#include <hal/interrupts.h>
#include <hal/exceptions.h>
#include <kernel/exception.h>
#include <printf.h>

void kernel_init_tty() {
    if (!drivers_console_fdt_probe())
        hal_halt();

    printf("[kernel] early console initialized\n");
}

void kernel_init() {
    kernel_init_memory();

    printf("[kernel] initializing HAL\n");
    hal_init();

    printf("[kernel] enabling interrupts and registering exception handlers\n");
    hal_enable_interrupts();
    hal_set_memory_exception_handler(exception_memory_error);

    kernel_init_drivers();

    printf("[kernel] init done\n");
}