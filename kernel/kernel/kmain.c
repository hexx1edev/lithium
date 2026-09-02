#include "kernel/memory/ramdisk/cpio.h"
#include "kernel/panic.h"
#include <kernel/ramdisk.h>
#include <kernel/init.h>
#include <kernel/memory.h>
#include <libfdt.h>

#include <printf.h>
#include <kernel/boot_info.h>

#include <hal/hal.h>
#include <drivers/remap.h>

boot_info _boot_info = {};
boot_info* info = NULL;

extern char __kernel_start[];
extern char __kernel_end[];

_Noreturn void kmain(const boot_info* _info) {
    memcpy(&_boot_info, _info, sizeof(boot_info));
    info = &_boot_info;

    drivers_relocate(info->load_addr);

    kernel_init_tty();

    printf("\nWelcome to lithium!\n\n");

    printf("[kernel] loaded at address 0x%llx\n", info->load_addr);

    kernel_init();

    kernel_search_ramdisk();

    cpio_handle_t init = cpio_lookup("init");

    if (init.ptr == NULL) {
        init = cpio_lookup("bin/init");
        if (init.ptr == NULL)
            panic("[kernel] no init executable found in ramdisk\n");
    }

    printf("[kernel] running init\n");

    hal_halt();
}
