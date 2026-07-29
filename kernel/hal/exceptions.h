#pragma once

#include <stdint.h>

typedef void (*hal_memory_exception_handler)(const char*, uint64_t, uint64_t);

void hal_set_memory_exception_handler(hal_memory_exception_handler handler);