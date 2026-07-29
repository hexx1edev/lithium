.section .text.init
.global _start

_start:
    bnez a0, halt   // halt on other harts, so we won't crash on multi-core systems

    la sp, _stack_top

    /* a0 = hartid */
    /* a1 = dtb */

    call arch_entry

halt:
    wfi
    j halt