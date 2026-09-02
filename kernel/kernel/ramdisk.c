#include "ramdisk.h"
#include <printf.h>
#include <kernel/memory/ramdisk/cpio.h>
#include <kernel/boot_info.h>
#include <kernel/panic.h>

void kernel_search_ramdisk() {
    printf("[kernel] searching for ramdisk\n");

    const void* fdt = kernel_get_boot_info()->fdt;

    if (!cpio_parse_fdt((void*)fdt)) {
        panic("[kernel] failed to find CPIO ramdisk\n");
    }
}