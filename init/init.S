.section .rodata
msg:
    .ascii "Hello from init!!!\n"
    .equ msg_len, . - msg

.section .text
.global _start
_start:
    la   a0, msg
    li   a1, msg_len
    li   a7, 0          // SYSCALL_WRITE
    ecall

.hang:
    j    .hang
