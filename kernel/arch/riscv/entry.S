.section .text.init
.global _header
.global _start

_header:
    jal     x0, _start           // jump
    .ascii  "LITB"               // magic
    .byte   0                    // version
_fdt:
    .dword  0                    // fdt
_ramdisk_start:
    .dword  0                    // ramdisk_start
_ramdisk_end:
    .dword  0                    // ramdisk_end
_flag:
    .byte   0                    // flag: U-Boot will set it to 1

_start:
    bnez a0, halt   // halt on other harts, so we won't crash on multi-core systems

    la sp, _stack_top

    /* a0 = hartid */
    /* a1 = dtb */

    call arch_entry

halt:
    wfi
    j halt