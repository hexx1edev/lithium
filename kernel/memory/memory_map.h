#pragma once

#include <stdint.h>

#define MEMORY_MAX_REGIONS 16

typedef struct {
    uint64_t start;
    uint64_t size;
} memory_region_t;

typedef struct {
    uint8_t regions_count;
    memory_region_t regions[MEMORY_MAX_REGIONS];
    uint64_t memory_size;
} memory_map_t;