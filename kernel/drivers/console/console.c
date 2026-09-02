#include "console.h"

#include <stdint.h>

#include <kernel/memory.h>

void (*_console_drivers[16])(char) = {};
size_t _console_drivers_count = 0;

void console_register(void (*putc)(char)) {
    if (_console_drivers_count >= sizeof(_console_drivers) / sizeof(_console_drivers[0]))
        return;

    _console_drivers[_console_drivers_count++] = putc;
}

void console_putc(char c) {
    for (size_t i = 0; i < _console_drivers_count; i++)
        _console_drivers[i](c);
}

void console_write(const char* buffer, size_t size) {
    for (size_t i = 0; i < size; i++)
        console_putc(buffer[i]);
}

void console_remap() {
    for (size_t i = 0; i < _console_drivers_count; i++)
        _console_drivers[i] = (void (*)(char))(uintptr_t)PA2VA((uint64_t)(uintptr_t)_console_drivers[i]);
}