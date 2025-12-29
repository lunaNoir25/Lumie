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

#include <stddef.h>
#include "screen.h"

extern char _binary_font_psf_start[];
static struct limine_framebuffer *target_fb;
static int cursor_x = 0;
static int cursor_y = 0;

void screen_init(struct limine_framebuffer *fb) {
    target_fb = fb;
}

void put_char(uint8_t c, int cx, int cy, uint32_t fg) {
    psf1_header_t *font = (psf1_header_t *)_binary_font_psf_start;
    uint8_t *glyph = (uint8_t *)_binary_font_psf_start + sizeof(psf1_header_t) + (c * font->charsize);
    uint32_t *fb_ptr = (uint32_t *)target_fb->address;

    for (uint32_t y = 0; y < font->charsize; y++) {
        for (uint32_t x = 0; x < 8; x++) {
            if ((*glyph & (0x80 >> x)) > 0) {
                fb_ptr[(cy + y) * (target_fb->pitch / 4) + (cx + x)] = fg;
            }
        }
        glyph++;
    }
}

void kprint(const char *str, uint32_t color) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\b') {
            backspace();
            continue;
        }


        if (str[i] == '\n') {
            cursor_x = 0;
            cursor_y += 16;
            continue;
        }

        if (cursor_x * 8 >= target_fb->width) {
            cursor_x = 0;
            cursor_y += 16;
        }
        
        if (cursor_y + 16 <= target_fb->height) {
            put_char(str[i], cursor_x * 8, cursor_y, color);
            cursor_x++;
        }
    }
}

void backspace() {
    if (cursor_x == 0 && cursor_y == 0) return;

    if (cursor_x > 0) {
        cursor_x--;
    } else {
        return;
    }

    uint32_t *fb_ptr = (uint32_t *)target_fb->address;
    for (int y = 0; y < 16; y++) {
        for (int x = 0; x < 8; x++) {
            int row = cursor_y + y;
            int col = (cursor_x * 8) + x;

            if (row < target_fb->height && col < target_fb->width) {
                fb_ptr[(cursor_y + y) * (target_fb->pitch / 4) + (cursor_x * 8 + x)] = 0x000000;
            }
        }
    }
}

void clear(uint32_t color) {
    uint32_t *fb_ptr = (uint32_t *)target_fb->address;
    for (size_t i = 0; i < target_fb->width * target_fb->height; i++) {
        fb_ptr[i] = color;
    }
    cursor_x = 0;
    cursor_y = 0;
}