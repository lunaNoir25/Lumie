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


#include "tar.h"
#include "../drivers/screen.h"
#include "../lib/string.h"

static void* tar_start;

static size_t octal_to_int(const char *in) {
    size_t size = 0;
    while (*in >= '0' && *in <= '7') {
        size = size * 8 + (*in++ - '0');
    }
    return size;
}

void tar_init(void* address) {
    tar_start = address;
}

char* tar_get_file(const char* filename, size_t* out_size) {
    tar_header_t* header = (tar_header_t*)tar_start;

    while (header->name[0] != '\0') {

        size_t size = octal_to_int(header->size);

        if (strcmp(header->name, filename) == 0 || 
           (header->name[0] == '.' && header->name[1] == '/' && strcmp(header->name + 2, filename) == 0)) {
            *out_size = size;
            return (char*)header + 512;
        }

        size_t aligned_size = ((size + 511) / 512) * 512;
        header = (tar_header_t*)((uintptr_t)header + 512 + aligned_size);

        if ((uintptr_t)header - (uintptr_t)tar_start > 1024 * 1024 * 10) break;
    }
    return NULL;
}

void tar_list_files() {
    tar_header_t* header = (tar_header_t*)tar_start;
    
    while (header->name[0] != '\0') {
        kprint(header->name, 0xFFFF00);
        kprint("\n", 0xFFFFFF);

        size_t size = octal_to_int(header->size);
        
        size_t aligned_data_size = ((size + 511) / 512) * 512;
        
        header = (tar_header_t*)((uintptr_t)header + 512 + aligned_data_size);

        if ((uintptr_t)header - (uintptr_t)tar_start > 1024 * 1024 * 10) break;
    }
}