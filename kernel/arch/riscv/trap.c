#include "trap.h"

#include <printf.h>
#include <stdbool.h>

#include "csr.h"
#include "plic.h"
#include <kernel/panic.h>

extern void trap_entry();

#define SCAUSE_INTERRUPT_BIT (1UL << 63)
#define SCAUSE_CODE_MASK (~SCAUSE_INTERRUPT_BIT)
#define IRQ_SUPERVISOR_EXTERNAL 9

hal_memory_exception_handler memoryExceptionHandler = NULL;

const char* scause_to_string(uint64_t scause) {
    switch (scause) {
        case 0:  return "instruction address misaligned";
        case 1:  return "instruction access fault";
        case 2:  return "illegal instruction";
        case 3:  return "breakpoint";
        case 4:  return "load address misaligned";
        case 5:  return "load access fault";
        case 6:  return "store/AMO address misaligned";
        case 7:  return "store/AMO access fault";
        case 8:  return "environment call from U-mode";
        case 9:  return "environment call from S-mode";
        case 10: return "reserved";
        case 11: return "environment call from M-mode";
        case 12: return "instruction page fault";
        case 13: return "load page fault";
        case 14: return "reserved";
        case 15: return "store/AMO page fault";
        case 16: return "double trap";
        case 17: return "reserved";
        case 18: return "software check";
        case 19: return "hardware error";
        default: return "unknown exception";
    }
}

void trap_init() {
    csr_write_stvec((uint64_t)trap_entry);
    csr_set_sie(CSR_SIE_SEIE);
}

void trap_dispatch(struct trap_frame* tf) {
    uint64_t scause = csr_read_scause();
    bool is_interrupt = (scause & SCAUSE_INTERRUPT_BIT) != 0;
    uint64_t code = scause & SCAUSE_CODE_MASK;

    if (is_interrupt)
    {
        if (code == IRQ_SUPERVISOR_EXTERNAL)
            plic_handle_interrupt();
        else
            printf("[trap] unexpected interrupt, scause=%lx\n", (unsigned long)scause);

        return;
    }

    uint64_t stval = csr_read_stval();

    if ((scause == 5 || scause == 7 || scause == 12 || scause == 13 || scause == 15) && memoryExceptionHandler != NULL) {
        memoryExceptionHandler(scause_to_string(scause), tf->sepc, stval);
        return;
    }

    panic("[trap] unhandled exception scause=%ld sepc=%lx stval=%lx\n",
          (unsigned long)scause, (unsigned long)tf->sepc, (unsigned long)stval);
}

void trap_set_memory_exception_handler(hal_memory_exception_handler handler) {
    memoryExceptionHandler = handler;
}