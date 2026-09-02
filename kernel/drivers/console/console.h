#pragma once

#include <stddef.h>

void console_register(void (*putc)(char));
void console_putc(char c);
void console_write(const char* buffer, size_t size);
void console_remap();