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

#include "keyboard.h"
#include "io.h"

static char* current_layout;

char en_us[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*',
    '0', ' ', 0
};

char fr_ca[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '^', 0x87, '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 0x8A, 0x85,  0,
   0x97, 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', 0x82,   0, '*',
    0, ' ', 0
};

void keyboard_init(char* layout) {
    current_layout = layout;

    while (inb(0x64) & 1) inb(0x60);

    outb(0x64, 0xAE);

    outb(0x64, 0x20);
    while (!(inb(0x64) & 1));
    uint8_t cb = inb(0x60);
    cb |= 1;
    cb &= ~0x10;
    
    outb(0x64, 0x60);
    while (inb(0x64) & 2);
    outb(0x60, cb);
}

static char last_char = '\0';

char get_last_char() {
    char c = last_char;
    last_char = '\0';
    return c;
}

static int shift_active = 0;

char apply_shift(char c) {
    if (c >= 'a' && c <= 'z') {
        return c - 32;
    }

    if (current_layout == en_us) {
        switch (c) {
        case '1': return '!';
        case '2': return '@';
        case '3': return '#';
        case '4': return '$';
        case '5': return '%';
        case '6': return '^';
        case '7': return '&';
        case '8': return '*';
        case '9': return '(';
        case '0': return ')';
        case '-': return '_';
        case '=': return '+';
        case '[': return '{';
        case ']': return '}';
        case ';': return ':';
        case '\'': return '\"';
        case '`': return '~';
        case '\\': return '|';
        case ',': return '<';
        case '.': return '>';
        case '/': return '?';
        }
        return c;
    } else if (current_layout == fr_ca) {
        switch ((unsigned char)c) {
            case '1': return '!';
            case '2': return '\"';
            case '3': return '/';
            case '4': return '$';
            case '5': return '%';
            case '6': return '?';
            case '7': return '&';
            case '8': return '*';
            case '9': return '(';
            case '0': return ')';
            case '-': return '_';
            case '=': return '+';

            case 0x85: return 0xB7;
            case 0x8A: return 0xD4;
            case 0x82: return 0x90;
            case 0x87: return 0x80;
            case 0x97: return 0xD6;
            
            case '^':  return 0xA8;
            case ';':  return ':';
            case ',':  return '<';
            case '.':  return '>';
            case '\'': return '\"';
        }
        return c;
    } else {
        return c;
    }
}

char apply_layout(uint8_t scancode) {
    char c = current_layout[scancode];

    if (shift_active) {
        return apply_shift(c);
    }

    return c;
}

static volatile char last_interrupt_char = '\0';

void keyboard_handler() {
    uint8_t scancode = inb(0x60);

    if (scancode == 0x2A || scancode == 0x36) {
        shift_active = 1;
        return;
    }

    if (scancode == 0xAA || scancode == 0xB6) {
        shift_active = 0;
        return;
    }

    if (scancode < 0x80) {
        last_interrupt_char = apply_layout(scancode);
    }
}


char keyboard_get_char() {
    char c = last_interrupt_char;
    last_interrupt_char = '\0';
    return c;
}