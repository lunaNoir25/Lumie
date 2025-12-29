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


#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include <stddef.h>

void readline(char* buffer, size_t max_len) {
    size_t i = 0;

    while (1) {
        char c = keyboard_get_char();
        if (!c) continue;

        if (c == '\n') {
            kprint_char('\n', 0xFFFFFF);
            break;
        }

        if (c == '\b') {
            if (i > 0) {
                i--;
                buffer[i] = '\0';
                backspace();
            }
            continue;
        }

        if (i < max_len - 1) {
            buffer[i++] = c;
            kprint_char(c, 0xFFFFFF);
        }
    }

    buffer[i] = '\0';
}
