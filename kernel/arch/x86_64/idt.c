/*  
    Copyright (C) 2026 Luna Moonlit Noir <lunaNoir.sk@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <stdint.h>
#include "idt.h"
#include "../../drivers/keyboard.h"
#include "../../drivers/screen.h"
#include "../../drivers/io.h"

extern void load_idt(void* idtr);

struct idt_entry {
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t  ist;
    uint8_t  attributes;
    uint16_t isr_mid;
    uint32_t isr_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

__attribute__((aligned(0x10))) 
static struct idt_entry idt[256];
static struct idt_ptr idtr;

void idt_set_descriptor(uint8_t vector, void* isr, uint8_t flags) {
    struct idt_entry* descriptor = &idt[vector];

    descriptor->isr_low    = (uint64_t)isr & 0xFFFF;
    descriptor->kernel_cs  = 0x08;
    descriptor->ist        = 0;
    descriptor->attributes = flags;
    descriptor->isr_mid    = ((uint64_t)isr >> 16) & 0xFFFF;
    descriptor->isr_high   = ((uint64_t)isr >> 32) & 0xFFFFFFFF;
    descriptor->reserved   = 0;
}

void interrupt_handler(struct interrupt_frame* frame) {
    uint64_t vector = frame->vector;

    if (vector == 33) {
        keyboard_handler();
    } else if (vector == 32) {} else {}

    if (vector >= 32 && vector < 48) {
        if (vector >= 40) {
            outb(0xA0, 0x20);
        }
        outb(0x20, 0x20);
    }
}

inline void pic_remap() {
    uint8_t a1, a2;

    a1 = inb(0x21);
    a2 = inb(0xA1);

    outb(0x20, 0x11); io_wait();
    outb(0xA0, 0x11); io_wait();
    
    outb(0x21, 0x20); io_wait();
    outb(0xA1, 0x28); io_wait();
    
    outb(0x21, 0x04); io_wait();
    outb(0xA1, 0x02); io_wait();
    
    outb(0x21, 0x01); io_wait();
    outb(0xA1, 0x01); io_wait();

    outb(0x21, a1);
    outb(0xA1, a2);

    outb(0x21, 0x00); io_wait();
    outb(0xA1, 0x00); io_wait();
}

void idt_init() {
    idtr.base = (uintptr_t)&idt[0];
    idtr.limit = (uint16_t)sizeof(struct idt_entry) * 256 - 1;

    extern void isr0();
    extern void isr1();
    extern void isr8();
    extern void isr14();
    extern void isr32();
    extern void isr33();
    extern void isr39();

    idt_set_descriptor(0, isr0, 0x8E);
    idt_set_descriptor(1, isr1, 0x8E);
    idt_set_descriptor(8, isr8, 0x8E);
    idt_set_descriptor(14, isr14, 0x8E);
    idt_set_descriptor(32, isr32, 0x8E);
    idt_set_descriptor(33, isr33, 0x8E);
    idt_set_descriptor(39, isr39, 0x8E);

    pic_remap();

    load_idt(&idtr);

    kprint("IDT Loaded.\n", 0x00FF00);
}