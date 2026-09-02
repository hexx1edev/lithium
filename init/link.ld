ENTRY(_start)

SECTIONS {
    . = 0x10000;

    .text : {
        *(.text*)
    }
    . = ALIGN(4096);

    .rodata : {
        *(.rodata*)
    }
    . = ALIGN(4096);

    .data : {
        *(.data*)
    }
    . = ALIGN(4096);

    .bss : {
        *(.bss*)
        *(COMMON)
    }
    . = ALIGN(4096);
}
