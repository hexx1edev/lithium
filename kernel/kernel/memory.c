#include "memory.h"
#include <memory/memory_map.h>
#include <memory/parse_fdt.h>
#include <memory/pmm/pmm.h>
#include <printf.h>
#include <memory.h>
#include <stdint.h>
#include <libfdt.h>
#include <kernel/boot_info.h>

#include <hal/mmu.h>
#include <hal/hal.h>

#include <kernel/arch.h>
#include <drivers/remap.h>

extern char __kernel_start[];
extern char __kernel_end[];
extern char __text_start[];
extern char __text_end[];
extern char __data_start[];
extern char __data_end[];
extern char __rodata_start[];
extern char __rodata_end[];
extern char __bss_start[];
extern char __bss_end[];
extern char _stack_top[];
extern char _stack_bottom[];
extern char __trampoline_start[];
extern char __trampoline_end[];

void kernel_init_memory() {
    uint64_t kernel_start = (uint64_t)__kernel_start;
    uint64_t kernel_end   = (uint64_t)__kernel_end;

    printf("[kernel] parsing memory map\n");
    memory_map_t mem_map = parse_memory_map_fdt((void*)info->fdt);

    uint64_t mem_mib = mem_map.memory_size / 1024 / 1024;
    printf("[kernel] memory size: %lld bytes / %lld MiB\n", mem_map.memory_size, mem_mib);

    printf("[kernel] initializing pmm\n");
    pmm_init(mem_map, kernel_end);

    printf("[kernel] reserving kernel memory\n");
    pmm_reserve(kernel_start, kernel_end - kernel_start);

    printf("[kernel] reserving FDT memory\n");
    uint32_t fdt_size = fdt_totalsize((void*)info->fdt);
    pmm_reserve((uint64_t)info->fdt, fdt_size);

    hal_reserve_memory();

    printf("[kernel] initializing MMU\n");
    hal_mmu_init();

    printf("[kernel] mapping kernel memory\n");
    uint64_t text_start = (uint64_t)__text_start;
    uint64_t text_end = (uint64_t)__text_end;
    uint64_t data_start = (uint64_t)__data_start;
    uint64_t data_end = (uint64_t)__data_end;
    uint64_t rodata_start = (uint64_t)__rodata_start;
    uint64_t rodata_end = (uint64_t)__rodata_end;
    uint64_t bss_start = (uint64_t)__bss_start;
    uint64_t bss_end = (uint64_t)__bss_end;
    uint64_t stack_top = (uint64_t)_stack_top;
    uint64_t stack_bottom = (uint64_t)_stack_bottom;

    hal_mmu_map(
        PA2VA(text_start),
        text_start,
        text_end - text_start,
        PERM_KERNEL_CODE
    );

    hal_mmu_map(
        PA2VA(data_start),
        data_start,
        data_end - data_start,
        PERM_KERNEL_DATA
    );
    
    hal_mmu_map(
        PA2VA(rodata_start),
        rodata_start,
        rodata_end - rodata_start,
        PERM_KERNEL_RODATA
    );

    hal_mmu_map(
        PA2VA(bss_start),
        bss_start,
        bss_end - bss_start,
        PERM_KERNEL_DATA
    );

    hal_mmu_map(
        PA2VA(stack_bottom),
        stack_bottom,
        stack_top - stack_bottom,
        PERM_KERNEL_DATA
    );

    uint64_t ram_start = mem_map.regions[0].start;
    uint64_t ram_end = ram_start + mem_map.memory_size;

    if (kernel_start > ram_start) {
        hal_mmu_map(PA2VA(ram_start), ram_start, kernel_start - ram_start, PERM_KERNEL_DATA);
    }
    if (ram_end > kernel_end) {
        hal_mmu_map(PA2VA(kernel_end), kernel_end, ram_end - kernel_end, PERM_KERNEL_DATA);
    }

    uint64_t trampoline_start = (uint64_t)__trampoline_start;
    uint64_t trampoline_end = (uint64_t)__trampoline_end;

    hal_mmu_map(
        trampoline_start,
        trampoline_start,
        trampoline_end - trampoline_start,
        PERM_KERNEL_CODE
    );

    hal_mmu_enable(2);

    info = (boot_info*)(uintptr_t)PA2VA((uint64_t)info);
    info->fdt = (const void*)(uintptr_t)PA2VA((uint64_t)info->fdt);

    pmm_remap();

    // tell drivers to map their MMIO
    drivers_map_memory();
}