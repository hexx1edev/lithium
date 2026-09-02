#include "write.h"
#include <drivers/console/console.h>

void syscall_write(const char* data, size_t size) {
    console_write(data, size);
}