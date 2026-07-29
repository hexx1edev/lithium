#include <printf.h>
#include <memory.h>

#include <drivers/probe/early_probe.h>
#include <drivers/probe/probe.h>
#include <hal/hal.h>
#include <hal/interrupts.h>
#include <kernel/boot_info.h>

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

    printf("[kernel] probing drivers\n");
    drivers_probe();

    printf("[kernel] enabling interrupts\n");
    hal_enable_interrupts();

    printf("[kernel] init done\n");

    hal_halt();
}
