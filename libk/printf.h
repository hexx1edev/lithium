#pragma once

#include <stdarg.h>
#include <stddef.h>

typedef void (*printf_callback)(char);

#define PRINTF_DISABLE_SUPPORT_FLOAT

#define printf printf_
int printf_(const char* format, ...);

#define sprintf sprintf_
int sprintf_(char* buffer, const char* format, ...);

#define snprintf  snprintf_
#define vsnprintf vsnprintf_
int  snprintf_(char* buffer, size_t count, const char* format, ...);
int vsnprintf_(char* buffer, size_t count, const char* format, va_list va);

#define vprintf vprintf_
int vprintf_(const char* format, va_list va);

void printf_set_callback(printf_callback callback);