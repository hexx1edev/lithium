#include "panic.h"
#include <hal/hal.h>

#include <printf.h>
#include <stdarg.h>

_Noreturn void panic(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    hal_halt();
}
