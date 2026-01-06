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

void utoa_hex(uint64_t n, char* str) {
    const char* hex_digits = "0123456789ABCDEF";
    
    str[0] = '0';
    str[1] = 'x';

    for (int i = 15; i >= 0; i--) {
        str[i + 2] = hex_digits[n & 0xF];
        n >>= 4;
    }

    str[18] = '\0';
}

void screen_init(struct limine_framebuffer *fb) {
    target_fb = fb;
}

void put_char(uint8_t c, int cx, int cy, uint32_t fg) {
    psf1_header_t *font = (psf1_header_t *)_binary_font_psf_start;
    uint8_t *glyph = (uint8_t *)_binary_font_psf_start + sizeof(psf1_header_t) + (c * font->charsize);
    uint32_t *fb_ptr = (uint32_t *)target_fb->address;

    int pixels_drawn = 0;

    for (uint32_t y = 0; y < font->charsize; y++) {
        for (uint32_t x = 0; x < 8; x++) {
            if ((*glyph & (0x80 >> x)) > 0) {
                fb_ptr[(cy + y) * (target_fb->pitch / 4) + (cx + x)] = fg;
                pixels_drawn++;
            }
        }
        glyph++;
    }
}

void put_char_bg(uint8_t c, int cx, int cy, uint32_t fg, uint32_t bg) {
    psf1_header_t *font = (psf1_header_t *)_binary_font_psf_start;
    uint8_t *glyph = (uint8_t *)_binary_font_psf_start + sizeof(psf1_header_t) + (c * font->charsize);
    uint32_t *fb_ptr = (uint32_t *)target_fb->address;

    for (uint32_t y = 0; y < font->charsize; y++) {
        for (uint32_t x = 0; x < 8; x++) {
            if ((*glyph & (0x80 >> x)) > 0) {
                fb_ptr[(cy + y) * (target_fb->pitch / 4) + (cx + x)] = fg;
            } else {
                fb_ptr[(cy + y) * (target_fb->pitch / 4) + (cx + x)] = bg;
            }
        }
        glyph++;
    }
}

void draw_rect(int x, int y, int width, int height, uint32_t color) {
    uint32_t *fb_ptr = (uint32_t *)target_fb->address;
    for (int i = y; i < y + height; i++) {
        for (int j = x; j < x + width; j++) {
            if (j >= 0 && j < target_fb->width && i >= 0 && i < target_fb->height) {
                fb_ptr[i * (target_fb->pitch / 4) + j] = color;
            }
        }
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

void kprint_char(char c, uint32_t color) {
    if (c == '\b') {
        backspace();
        return;
    }

    if (c == '\n') {
        cursor_x = 0;
        cursor_y += 16;
        return;
    }

    if (cursor_x * 8 >= target_fb->width) {
        cursor_x = 0;
        cursor_y += 16;
    }

    if (cursor_y + 16 <= target_fb->height) {
        put_char((uint8_t)c, cursor_x * 8, cursor_y, color);
        cursor_x++;
    }
}

void kprint_at(const char* str, uint32_t fg, uint32_t bg, int x, int y) {
    if (!target_fb) return;

    for (int i = 0; str[i] != '\0'; i++) {
        int screen_x = (x + i) * 8;
        int screen_y = y * 16;

        if (screen_x + 8 <= target_fb->width && screen_y + 16 <= target_fb->height) {
            put_char_bg((uint8_t)str[i], screen_x, screen_y, fg, bg);
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