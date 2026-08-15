#include "ramdisk.h"
#include <printf.h>
#include <memory/ramdisk/cpio.h>
#include <kernel/boot_info.h>
#include <kernel/panic.h>

void kernel_search_ramdisk() {
    printf("[kernel] searching for ramdisk\n");

    const void* fdt = kernel_get_boot_info()->fdt;

    if (!cpio_parse_fdt((void*)fdt)) {
        panic("[kernel] failed to find CPIO ramdisk! cannot start!\n");
    }

    cpio_handle_t handle = cpio_lookup("message.txt");
    if (handle.ptr == NULL) {
        printf("[kernel] no cpio message found!\n");
    } else {
        printf("%s\n", (char*)handle.ptr);
    }
}