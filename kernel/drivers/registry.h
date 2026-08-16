#pragma once

#include <drivers/types.h>
#include <drivers/ns16550a/ns16550a.h>

#define DRIVERS_COUNT 1
#define CONSOLE_DRIVERS_COUNT 1

extern driver_t drivers[DRIVERS_COUNT];
extern console_driver_t console_drivers[CONSOLE_DRIVERS_COUNT];
