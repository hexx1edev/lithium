#include <kernel/init.h>
#include <libfdt.h>

#include <printf.h>
#include <kernel/boot_info.h>

#include <hal/hal.h>

boot_info _boot_info = {};
boot_info* info = NULL;

_Noreturn void kmain(const boot_info* _info) {
    memcpy(&_boot_info, _info, sizeof(boot_info));
    info = &_boot_info;

    kernel_init_tty();

    printf("\nWelcome to lithium!\n\n");

    kernel_init();

    printf("[kernel] back to kmain!");

    hal_halt();
}
