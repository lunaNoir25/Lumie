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
#include "drivers/tar.h"
#include "includes/limine.h"
#include "drivers/screen.h"
#include "drivers/keyboard.h"
#include "drivers/timer.h"
#include "lib/input.h"
#include "lib/string.h"
#include "shell.h"

#if __x86_64__
#include "arch/x86_64/gdt.h"
#include "arch/x86_64/idt.h"
#endif

static volatile struct limine_module_request module_request = {
    .id = LIMINE_MODULE_REQUEST,
    .revision = 0
};

static volatile struct limine_framebuffer_request framebuffer_request = {
    .id = LIMINE_FRAMEBUFFER_REQUEST,
    .revision = 0
};

void kmain(void) {
    if (framebuffer_request.response == NULL || framebuffer_request.response->framebuffer_count < 1) {
        while(1) { __asm__("hlt"); }
    }

    struct limine_framebuffer *fb = framebuffer_request.response->framebuffers[0];

    timer_init(100);
    
    screen_init(fb);
    clear(0x000000);

    gdt_init();

    idt_init();

    if (module_request.response != NULL && module_request.response->module_count > 0) {
        struct limine_file *init_file = module_request.response->modules[0];
        tar_init(init_file->address);

        size_t f_size;
        char* motd = tar_get_file("motd.txt", &f_size);
        if (motd) {
            kprint("Found MOTD.\n", 0x0000FF);
        }
    }

    keyboard_init(en_us);

    __asm__ volatile ("sti");
    kprint("Interrupts enabled.\n", 0x00FF00);


    char input_buffer[128];

    kprint("\nLumie v26.1-alpha\n\n", 0xFFFFFF);

    kernel_shell();
}