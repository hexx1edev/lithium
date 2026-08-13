#pragma once

#include <stdbool.h>

typedef enum {
    FDT_DEVICE,
    PCI_DEVICE
} device_t_type_t;

typedef struct {
    device_t_type_t type;
    const char* fdt_compatible;
} device_t;

typedef struct {
    device_t device;
    bool (*fdt_console_init)(void* fdt, int node);
    void (*putc)(char c);
} console_driver_t;

typedef struct {
    device_t device;
    bool (*fdt_init)(void* fdt, int node);
    void (*map_memory)();
} driver_t;