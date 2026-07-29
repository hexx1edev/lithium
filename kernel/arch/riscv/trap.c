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
            printf("trap: unexpected interrupt, scause=%lx\n", (unsigned long)scause);

        return;
    }

    panic("trap: unhandled exception scause=%lx sepc=%lx stval=%lx\n",
          (unsigned long)scause, (unsigned long)tf->sepc, (unsigned long)csr_read_stval());
}
