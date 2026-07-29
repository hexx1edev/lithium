#include "exception.h"
#include <kernel/panic.h>

void exception_memory_error(const char* text, uint64_t instr, uint64_t address) {
    panic("[kernel] a memory access exception occured in kernel\n    description: %s\n    PC: 0x%016llx\n    memory address: 0x%016llx\n",
        text, instr, address);
}