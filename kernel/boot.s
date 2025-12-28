section .text
bits 64
global _start
extern kmain

_start:
    push rbp
    mov rbp, rsp

    call kmain

.hang:
    cli
    hlt
    jmp .hang