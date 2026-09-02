#pragma once

#include <stdint.h>

void syscall_handler(int syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3);