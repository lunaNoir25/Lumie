OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    . = 0xffffffff80000000;

    .text : {
        *(.text .text.*)
    }

    .rodata : {
        *(.rodata .rodata.*)
    }

    .data : {
        *(.data .data.*)
    }

    .font : ALIGN(16) {
        *(.font)
    }

    .bss : {
        *(.bss .bss.*)
        *(COMMON)
    }
}