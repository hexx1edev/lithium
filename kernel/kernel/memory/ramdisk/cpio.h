#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

typedef struct {
    void* ptr;
    size_t size;
} cpio_handle_t;

bool cpio_parse_fdt(void* fdt);
cpio_handle_t cpio_lookup(const char* path);