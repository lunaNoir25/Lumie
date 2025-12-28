#include "includes/limine.h"
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint8_t magic[2];
    uint8_t mode;
    uint8_t charsize;
} psf1_header_t;

extern char _binary_font_psf_start[];

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void put_char(struct limine_framebuffer *fb, char c, int cx, int cy, uint32_t fg) {
    psf1_header_t *font = (psf1_header_t *)_binary_font_psf_start;
    
    uint8_t *glyph = (uint8_t *)_binary_font_psf_start + sizeof(psf1_header_t) + (c * font->charsize);

    uint32_t *fb_ptr = (uint32_t *)fb->address;

    for (uint32_t y = 0; y < font->charsize; y++) {
        for (uint32_t x = 0; x < 8; x++) {
            if ((*glyph & (0x80 >> x)) > 0) {
                fb_ptr[(cy + y) * (fb->pitch / 4) + (cx + x)] = fg;
            }
        }
        glyph++;
    }
}

void kmain(void) {
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        while(1) { __asm__("hlt"); }
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];
    psf1_header_t *font = (psf1_header_t *)_binary_font_psf_start;

    if (font->magic[0] != 0x36 || font->magic[1] != 0x04) {
        for (size_t i = 0; i < fb->width * fb->height; i++) {
            ((uint32_t *)fb->address)[i] = 0xFF0000;
        }
        while(1) { __asm__("hlt"); }
    }

    for (size_t i = 0; i < fb->width * fb->height; i++) {
        ((uint32_t *)fb->address)[i] = 0x000000;
    }

    char *text = "Welcome to Lumie!";
    for (int i = 0; text[i] != '\0'; i++) {
        put_char(fb, text[i], 20 + (i * 8), 20, 0xFFFFFF);
    }

    while(1) { __asm__("hlt"); }
}