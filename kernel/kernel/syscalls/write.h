#pragma once

#include <stddef.h>

#define SYSCALL_WRITE 0

void syscall_write(const char* data, size_t size);