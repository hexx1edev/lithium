#include <stdint.h>

#include <kernel/boot_info.h>

_Noreturn void kmain(const boot_info* info);

_Noreturn void arch_entry(uint64_t hartid, void* fdt) {
    arch_boot_info archInfo = {
        .riscv_hartid = hartid
    };

    boot_info info = {
        .arch = archInfo,
        .fdt = fdt,
    };

    kmain(&info);
}
