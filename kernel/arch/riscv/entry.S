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
    .byte   0                    // flag: U-Boot will set it to 1, OpenSBI won't

_start:
    // detect load address
    auipc t0, 0
    lui   t1, %hi(_start)
    addi  t1, t1, %lo(_start)
    sub   a4, t0, t1

    la t0, _flag
    lb t1, 0(t0)
    bnez t1, .litboot

.no_litboot:
    bnez a0, halt   // halt on other harts, so we won't crash on multi-core systems
    li a2, 0
    li a3, 0
    j .boot

.litboot:
    li a0, 0

    la t0, _fdt
    ld a1, 0(t0)

    la t0, _ramdisk_start
    ld a2, 0(t0)

    la t0, _ramdisk_end
    ld a3, 0(t0)

.boot:
    la sp, _stack_top

    fence.i     // reset I-Cache

    /* a0 = hartid */
    /* a1 = fdt */
    /* a2 = ramdisk start */
    /* a3 = ramdisk end */
    /* a4 = load address */

    call arch_entry

halt:
    wfi
    j halt