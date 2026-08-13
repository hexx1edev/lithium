#include "pmm.h"
#include <memory.h>
#include <printf.h>
#include <memory/memory_map.h>
#include <kernel/memory.h>

static uint8_t* bitmap = NULL;
static uint64_t bitmap_size = 0;
static uint64_t phys_base = 0;   // Physical RAM start address (e.g., 0x80000000)
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;

static inline void bitmap_set(uint64_t page) {
    bitmap[page / 8] |= (1 << (page % 8));
}

static inline void bitmap_clear(uint64_t page) {
    bitmap[page / 8] &= ~(1 << (page % 8));
}

static inline bool bitmap_test(uint64_t page) {
    return (bitmap[page / 8] & (1 << (page % 8))) != 0;
}

void pmm_mark_region(uint64_t start, uint64_t size, bool used) {
    // 1. Guard against marking addresses below physical base RAM
    if (start < phys_base) {
        if (start + size <= phys_base) return;
        size -= (phys_base - start);
        start = phys_base;
    }

    // 2. Convert physical addresses to relative page indices
    uint64_t rel_start = start - phys_base;
    uint64_t rel_end   = rel_start + size;

    uint64_t start_page = PAGE_ALIGN_DOWN(rel_start) / PAGE_SIZE;
    uint64_t end_page   = PAGE_ALIGN_UP(rel_end) / PAGE_SIZE;

    // 3. Clamp end_page to total_pages to prevent off-by-one / out-of-bounds array access
    if (end_page > total_pages) {
        end_page = total_pages;
    }

    for (uint64_t page = start_page; page < end_page; page++) {
        bool current_state = bitmap_test(page);
        if (used && !current_state) {
            bitmap_set(page);
            used_pages++;
        } else if (!used && current_state) {
            bitmap_clear(page);
            used_pages--;
        }
    }
}

void pmm_init(memory_map_t map, uint64_t bitmap_phys_addr) {
    // set RAM physical base from the first memory region
    phys_base = map.regions[0].start;
    total_pages = map.memory_size / PAGE_SIZE;
    bitmap_size = (total_pages + 7) / 8; // round up byte size

    bitmap = (uint8_t*)bitmap_phys_addr;

    // mark all memory as a reserved
    memset(bitmap, 0xFF, bitmap_size);
    used_pages = total_pages;

    // unmark memory, that is usable
    for (int i = 0; i < map.regions_count; i++) {
        pmm_mark_region(map.regions[i].start, map.regions[i].size, false);
    }

    // reserve bitmap memory
    pmm_reserve((uint64_t)bitmap, bitmap_size);

    printf("[pmm] initialized: %llu total pages, %llu free pages\n", 
           total_pages, total_pages - used_pages);
}

// The bitmap lives in raw physical memory right after the kernel image, not
// in any statically-mapped section, but kernel_init_memory() maps the rest
// of RAM (everything past the kernel image) before enabling the MMU, so by
// the time this runs all that's left is repointing this to the high-half
// alias of the same memory.
void pmm_remap(void) {
    bitmap = (uint8_t*)(uintptr_t)PA2VA((uint64_t)bitmap);
}

void pmm_reserve(uint64_t start, uint64_t size) {
    pmm_mark_region(start, size, true);
}

void pmm_free_region(uint64_t start, uint64_t size) {
    pmm_mark_region(start, size, false);
}

void* pmm_alloc_page(void) {
    for (uint64_t page = 0; page < total_pages; page++) {
        if (!bitmap_test(page)) {
            bitmap_set(page);
            used_pages++;
            // Add phys_base to return a valid physical memory address
            return (void*)(phys_base + (page * PAGE_SIZE));
        }
    }
    printf("[pmm] out of memory!\n");
    return NULL;
}

void pmm_free_page(void* ptr) {
    uint64_t addr = (uint64_t)ptr;
    
    // Guard against unaligned or out-of-range pointers
    if (addr < phys_base || addr % PAGE_SIZE != 0) return;

    uint64_t page = (addr - phys_base) / PAGE_SIZE;
    if (page < total_pages && bitmap_test(page)) {
        bitmap_clear(page);
        used_pages--;
    }
}

void* pmm_alloc_pages(uint64_t size) {
    if (size == 0) return NULL;

    uint64_t pages = PAGE_ALIGN_UP(size) / PAGE_SIZE;

    if (pages == 1) {
        return pmm_alloc_page();
    }

    uint64_t free_count = 0;
    uint64_t start_page = 0;

    // contiguous search
    for (uint64_t page = 0; page < total_pages; page++) {
        if (!bitmap_test(page)) {
            // free page in a potential candidate block
            if (free_count == 0) {
                start_page = page;
            }

            free_count++;

            // block of pages found
            if (free_count == pages) {
                // mark all pages in the block as USED
                for (uint64_t p = start_page; p < start_page + pages; p++) {
                    bitmap_set(p);
                }
                
                used_pages += pages;

                // return the physical base address of the first page in the block
                return (void*)(phys_base + (start_page * PAGE_SIZE));
            }
        } else {
            // reset sequence counter
            free_count = 0;
        }
    }

    printf("[pmm] out of contiguous memory for %llu bytes (%llu pages)!\n", 
           (uint64_t)size, pages);
    return NULL;
}

void pmm_free_pages(void* ptr, size_t size) {
    uint64_t addr = (uint64_t)ptr;
    
    if (size == 0 || addr < phys_base || addr % PAGE_SIZE != 0) return;

    uint64_t pages_to_free = PAGE_ALIGN_UP(size) / PAGE_SIZE;
    uint64_t start_page = (addr - phys_base) / PAGE_SIZE;

    for (uint64_t page = start_page; 
         page < start_page + pages_to_free && page < total_pages; 
         page++) 
    {
        if (bitmap_test(page)) {
            bitmap_clear(page);
            used_pages--;
        }
    }
}