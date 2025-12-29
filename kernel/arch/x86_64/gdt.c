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
#include "../../drivers/screen.h"

extern void gdt_load(void* gdt_ptr);

struct gdt_entry {
    uint16_t limit_lot;
    uint16_t base_low;
    uint8_t base_middle;
    uint8_t access;
    uint8_t granularity;
    uint8_t base_high;
} __attribute__((packed));

struct gdt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

struct gdt_entry gdt[3];
struct gdt_ptr gdt_ptr;


void gdt_init() {
    gdt[0] = (struct gdt_entry){0, 0, 0, 0, 0, 0};
    gdt[1] = (struct gdt_entry){
        .limit_lot = 0,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x9A,
        .granularity = 0x20,
        .base_high = 0
    };
    gdt[2] = (struct gdt_entry){
        .limit_lot = 0,
        .base_low = 0,
        .base_middle = 0,
        .access = 0x92,
        .granularity = 0x00,
        .base_high = 0
    };

    gdt_ptr.limit = (sizeof(struct gdt_entry) * 3) - 1;
    gdt_ptr.base = (uint64_t)&gdt;

    char buf[20];
    kprint("GDT base: ", 0x0000FF);
    utoa_hex((uintptr_t)&gdt, buf);
    kprint(buf, 0x0000FF);
    kprint("\n", 0x0000FF);

    kprint("GDT ptr base: ", 0x0000FF);
    utoa_hex(gdt_ptr.base, buf);
    kprint(buf, 0x0000FF);
    kprint("\n", 0x0000FF);

    gdt_load(&gdt_ptr);

    kprint("GDT Loaded.\n", 0x00FF00);
}