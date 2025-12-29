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

#include "timer.h"
#include "io.h"

uint64_t system_ticks = 0;

void timer_init(uint32_t frequency) {
    uint32_t divisor = 1193182 / frequency;

    outb(0x43, 0x36);

    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

    outb(0x40, l);
    outb(0x40, h);
}

void timer_handler() {
    system_ticks++;
}

uint64_t get_uptime() {
    return system_ticks;
}

void sleep(uint64_t ms) {
    uint64_t start_ticks = system_ticks;
    uint64_t ticks_to_wait = ms / 10;

    while (system_ticks < start_ticks + ticks_to_wait) {
        __asm__("hlt");
    }
}

void pit_prepare_sleep(uint32_t ms) {
    uint32_t ticks = ms * 1193;
    if (ticks > 0xFFFF) ticks = 0xFFFF;

    outb(0x43, 0x30); 
    outb(0x40, (uint8_t)(ticks & 0xFF));
    outb(0x40, (uint8_t)((ticks >> 8) & 0xFF));
}

void pit_perform_sleep() {
    while (1) {
        outb(0x43, 0xE2);
        uint8_t status = inb(0x40);
        if (status & 0x80) break;
    }
}

void safe_sleep(uint32_t ms) {
    while (ms > 50) {
        pit_prepare_sleep(50);
        pit_perform_sleep();
        ms -= 50;
    }
    pit_prepare_sleep(ms);
    pit_perform_sleep();
}