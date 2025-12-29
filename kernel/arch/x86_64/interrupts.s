;  Copyright (C) 2026 Luna Moonlit Noir <lunaNoir.sk@gmail.com>
;
;    This program is free software: you can redistribute it and/or modify
;    it under the terms of the GNU General Public License as published by
;    the Free Software Foundation, either version 3 of the License, or
;    (at your option) any later version.
;
;    This program is distributed in the hope that it will be useful,
;    but WITHOUT ANY WARRANTY; without even the implied warranty of
;    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;    GNU General Public License for more details.
;
;    You should have received a copy of the GNU General Public License
;    along with this program.  If not, see <https://www.gnu.org/licenses/>.

extern interrupt_handler
global load_idt

load_idt:
    lidt [rdi]
    ret

%macro isr_no_err 1
global isr%1
isr%1:
    push 0
    push %1
    jmp isr_common
%endmacro

%macro isr_err 1
global isr%1
isr%1:
    push %1
    jmp isr_common
%endmacro

isr_common:
    push rbp
    mov rbp, rsp
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rdi, rsp
    call interrupt_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    pop rbp
    add rsp, 16
    iretq

isr_no_err 0
isr_no_err 1
isr_no_err 6
isr_err    8
isr_err    13
isr_err    14
isr_no_err 32
isr_no_err 33
isr_no_err 39