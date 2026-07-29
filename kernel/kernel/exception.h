#pragma once

#include <stdint.h>

void exception_memory_error(const char* text, uint64_t instr, uint64_t address);