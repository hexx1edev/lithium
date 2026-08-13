.section .text.trampoline, "ax"
.align 12
.global sv39_switch

# does the switch and fixes return address to make kernel continue run by virtual pc
# identity mapped
sv39_switch:
    csrw satp, a0
    sfence.vma zero, zero

    add sp, sp, a1
    add ra, ra, a1
    ret
