ENTRY(_start)

SECTIONS {
    . = 0x80200000;

    __kernel_start = .;

    .text : {
        KEEP(*(.text.init))
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        *(.bss*)
        *(COMMON)
    }

    . = ALIGN(16);
    _stack_bottom = .;
    . += 16K;
    _stack_top = .;

    __kernel_end = .;
}