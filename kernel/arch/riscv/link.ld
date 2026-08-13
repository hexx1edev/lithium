ENTRY(_start)

SECTIONS {
    . = 0x80200000;

    __kernel_start = .;

    __text_start = .;
    .text : {
        KEEP(*(.text.init))

        . = ALIGN(4096);
        __trampoline_start = .;
        KEEP(*(.text.trampoline))
        . = ALIGN(4096);
        __trampoline_end = .;

        *(.text*)
    }
    . = ALIGN(4096);
    __text_end = .;

    __rodata_start = .;
    .rodata : {
        *(.rodata*)
        *(.srodata*)
    }
    . = ALIGN(4096);
    __rodata_end = .;

    __data_start = .;
    .data : {
        *(.data*)
        *(.sdata*)
    }
    . = ALIGN(4096);
    __data_end = .;

    __bss_start = .;
    .bss : {
        *(.bss*)
        *(.sbss*) /* cland adds this sections sometimes */
        *(.scommon*)
        *(COMMON)
    }
    . = ALIGN(4096);
    __bss_end = .;

    _stack_bottom = .;
    . += 16K;
    _stack_top = .;

    __kernel_end = .;
}