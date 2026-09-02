#include "handler.h"
#include <kernel/syscalls/write.h>
#include <printf.h>

void syscall_handler(int syscall, uint64_t arg0, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (syscall) {
        case SYSCALL_WRITE:
            syscall_write((const char*)arg0, arg1);
            break;
        default:
            printf("[syscall] unknown syscall number: %d", syscall);
    }
}