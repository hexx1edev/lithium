#pragma once

#include <stdint.h>

void drivers_relocate(uint64_t delta);
void drivers_map_memory();