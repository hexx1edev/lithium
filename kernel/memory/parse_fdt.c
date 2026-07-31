#include "parse_fdt.h"
#include <memory/memory_map.h>
#include <libfdt.h>
#include <kernel/fdt_util.h>
#include <printf.h>

memory_map_t parse_memory_map_fdt(void* fdt) {
    memory_map_t map = {};

    int offset = -1;

    int root = fdt_path_offset(fdt, "/");

    int addr_cells = fdt_address_cells(fdt, root);
    int size_cells = fdt_size_cells(fdt, root);

    while ((offset = fdt_next_node(fdt, offset, NULL)) >= 0) {
        const char* type =
            fdt_getprop(fdt, offset, "device_type", NULL);

        if (!type || strcmp(type, "memory"))
            continue;

        int len;

        const fdt32_t* reg =
            fdt_getprop(fdt, offset, "reg", &len);

        if (!reg)
            continue;

        int cells_per_region =
            addr_cells + size_cells;

        int count =
            (len / sizeof(fdt32_t)) / cells_per_region;
        
        const fdt32_t* p = reg;

        printf("[memory] enumerating memory regions:\n");

        for (int i = 0; i < count; i++) {
            if (map.regions_count >= MEMORY_MAX_REGIONS) {
                printf("[memory] too many memory regions, stopping\n");
                break;
            }

            uint64_t addr = fdt_read_cells(p, addr_cells);
            p += addr_cells;

            uint64_t size = fdt_read_cells(p, size_cells);
            p += size_cells;

            printf("    region %d: start=%016llx size=%016llx\n", i, addr, size);

            map.regions[map.regions_count] = (memory_region_t){addr, size};
            map.regions_count++;
            map.memory_size += size;
        }
    }

    return map;
}