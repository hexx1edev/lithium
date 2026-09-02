#pragma once

#include <stddef.h>

typedef enum {
    ELF64_OK = 0,
    ELF64_ERR_MAGIC,
    ELF64_ERR_CLASS,
    ELF64_ERR_DATA,
    ELF64_ERR_MACHINE,
    ELF64_ERR_TYPE,
    ELF64_ERR_BOUNDS,
    ELF64_ERR_NOMEM,
    ELF64_ERR_MAP,
    ELF64_ERR_NO_LOAD
} elf64_status_t;

typedef struct {
    elf64_status_t status;
    void* ptr;
    size_t size;
} elf64_exec_handle_t;

elf64_exec_handle_t elf64_load_exec(void* ptr, size_t size);