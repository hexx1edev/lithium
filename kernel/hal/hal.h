#pragma once

#include <stdint.h>

#include <kernel/boot_info.h>

void hal_init();
_Noreturn void hal_halt();

void hal_reserve_memory();

void hal_update_stack();