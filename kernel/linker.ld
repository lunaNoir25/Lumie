OUTPUT_FORMAT(elf64-x86-64)
ENTRY(_start)

SECTIONS
{
    . = 0xffffffff80000000;

    .text : {
    *(.text .text.*)
    }

    . = ALIGN(0x1000);

    .rodata : {
        *(.rodata .rodata.*)
    }
    
    . = ALIGN(0x1000);

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