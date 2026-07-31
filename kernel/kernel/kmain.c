#include <memory/memory_map.h>
#include <memory/pmm/pmm.h>
#include <printf.h>
#include <memory.h>
#include <libfdt.h>

#include <hal/hal.h>
#include <drivers/fdt_probe.h>
#include <hal/interrupts.h>
#include <hal/exceptions.h>
#include <kernel/boot_info.h>
#include <kernel/exception.h>
#include <memory/parse_fdt.h>

boot_info _boot_info = {};
boot_info* info = NULL;

extern char __kernel_start[];
extern char __kernel_end[];

_Noreturn void kmain(const boot_info* _info) {
    memcpy(&_boot_info, _info, sizeof(boot_info));
    info = &_boot_info;

    // nothing can be logged until a console is up, so this comes first.
    if (!drivers_console_fdt_probe())
        hal_halt();

    printf("[kernel] early console initialized, fdt at 0x%016llx\n", info->fdt);

    uint64_t kernel_start = (uint64_t)__kernel_start;
    uint64_t kernel_end   = (uint64_t)__kernel_end;

    printf("[kernel] parsing memory map\n");
    memory_map_t mem_map = parse_memory_map_fdt((void*)_info->fdt);

    uint64_t mem_mib = mem_map.memory_size / 1024 / 1024;
    printf("[kernel] memory size: %lld bytes / %lld MiB\n", mem_map.memory_size, mem_mib);

    printf("[kernel] initializing pmm\n");
    pmm_init(mem_map, kernel_end);

    printf("[kernel] reserving kernel memory\n");
    pmm_reserve(kernel_start, kernel_end - kernel_start);

    printf("[kernel] reserving FDT memory\n");
    uint32_t fdt_size = fdt_totalsize((void*)info->fdt);
    pmm_reserve((uint64_t)_info->fdt, fdt_size);

    hal_reserve_memory();

    printf("[kernel] initializing HAL\n");
    hal_init();

    printf("[kernel] enabling interrupts and registering exception handlers\n");
    hal_enable_interrupts();
    hal_set_memory_exception_handler(exception_memory_error);

    printf("[kernel] probing drivers\n");
    drivers_fdt_probe();

    printf("[kernel] init done\n");

    hal_halt();
}
