#pragma once

#include <stdint.h>

#define CSR_SSTATUS_SIE (1UL << 1)
#define CSR_SIE_SEIE (1UL << 9)

static inline uint64_t csr_read_sepc() {
    uint64_t value;
    asm volatile("csrr %0, sepc" : "=r"(value));
    return value;
}

static inline uint64_t csr_read_scause() {
    uint64_t value;
    asm volatile("csrr %0, scause" : "=r"(value));
    return value;
}

static inline uint64_t csr_read_stval() {
    uint64_t value;
    asm volatile("csrr %0, stval" : "=r"(value));
    return value;
}

static inline void csr_write_stvec(uint64_t value) {
    asm volatile("csrw stvec, %0" ::"r"(value));
}

static inline void csr_set_sie(uint64_t mask) {
    asm volatile("csrs sie, %0" ::"r"(mask));
}

static inline void csr_set_sstatus(uint64_t mask) {
    asm volatile("csrs sstatus, %0" ::"r"(mask));
}
