#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <memory/memory_map.h>

#define PAGE_SIZE 4096
#define PAGE_ALIGN_DOWN(x) ((x) & ~(PAGE_SIZE - 1))
#define PAGE_ALIGN_UP(x)   (((x) + PAGE_SIZE - 1) & ~(PAGE_SIZE - 1))

void pmm_init(memory_map_t map, uint64_t free_bitmap_addr);
void pmm_mark_region(uint64_t start, uint64_t size, bool used);

void pmm_reserve(uint64_t start, uint64_t size);
void pmm_free_region(uint64_t start, uint64_t size);

void* pmm_alloc_page(void);
void pmm_free_page(void* ptr);

void* pmm_alloc_pages(uint64_t size);
void pmm_free_pages(void* ptr, uint64_t size);