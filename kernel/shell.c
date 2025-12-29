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

#include "shell.h"
#include "drivers/screen.h"
#include "drivers/tar.h"
#include "lib/string.h"
#include "lib/input.h"

typedef struct {
    const char* name;
    void (*func)(char* args);
} command_t;

void cmd_clear(char* args) {
    clear(0x000000);
}

void cmd_help(char* args){
    kprint("clear       Clear the screen.\n", 0xFFFFFF);
    kprint("ls          List files in ramdisk.\n", 0xFFFFFF);
    kprint("cat         Read a file.\n", 0xFFFFFF);
}

void cmd_ls(char* args) {
    tar_list_files();
}

void cmd_cat(char* args) {
    if (strlen(args) == 0) {
        kprint("Usage: ", 0xFF0000);
        kprint("cat <filename>\n", 0xFFFFFF);
        return;
    }

    size_t size;
    char* file = tar_get_file(args, &size);
    if (file) {
        for (size_t i = 0; i < size; i++) {
            char s[2] = {file[i], '\0'};
            kprint(s, 0x00FF00);
        }
        kprint("\n", 0xFFFFFF);
    } else {
        kprint("Provided file not found.\n", 0xFFFFFF);
    }
}

command_t commands[] = {
    {"clear", cmd_clear},
    {"help", cmd_help},
    {"ls", cmd_ls},
    {"cat", cmd_cat}
};

void kernel_shell() {
    char input[128];
    
    while (1) {
        kprint("> ", 0xFFFFFF);
        readline(input, 128);

        if (strlen(input) == 0) continue;

        char* args = "";
        for (int i = 0; input[i] != '\0'; i++) {
            if (input[i] == ' ') {
                input[i] = '\0';
                args = &input[i+1];
                break;
            }
        }

        int found = 0;
        for (int i = 0; i < sizeof(commands)/sizeof(command_t); i++) {
            if (strcmp(input, commands[i].name) == 0) {
                commands[i].func(args);
                found = 1;
                break;
            }
        }

        if (!found) {
            kprint("Kernel command \"", 0xFF0000);
            kprint(input, 0xFF0000);
            kprint("\" does not exist.\n", 0xFF0000);
        }
    }
}