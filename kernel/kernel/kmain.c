#include <printf.h>
#include <memory.h>

#include <drivers/probe/early_probe.h>
#include <drivers/probe/probe.h>
#include <hal/hal.h>
#include <hal/interrupts.h>
#include <hal/exceptions.h>
#include <kernel/boot_info.h>
#include <kernel/exception.h>

boot_info _boot_info = {};
boot_info* info = NULL;

_Noreturn void kmain(const boot_info* _info) {
    memcpy(&_boot_info, _info, sizeof(boot_info));
    info = &_boot_info;

    // Nothing can be logged until a console is up, so this comes first.
    if (!drivers_early_probe())
        hal_halt();

    printf("[kernel] early console up, fdt at %p\n", info->fdt);

    printf("[kernel] initializing HAL\n");
    hal_init();

    printf("[kernel] enabling interrupts and registering exception handlers\n");
    hal_enable_interrupts();
    hal_set_memory_exception_handler(exception_memory_error);

    printf("[kernel] probing drivers\n");
    drivers_probe();

    printf("[kernel] init done\n");

    *(uint8_t*)(0xFFFFFFFFFFFF) = 0;

    hal_halt();
}
