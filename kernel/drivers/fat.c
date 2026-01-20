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

#include "fat.h"
#include "ata.h"
#include "screen.h"

uint8_t boot_sector[512];

void fat_init() {
    ata_read_sector(0, (uint16_t*)boot_sector);
    
    fat_bpb_t* bpb = (fat_bpb_t*)boot_sector;
    
    uint32_t root_dir_lba = bpb->reserved_sectors + (bpb->fat_count * bpb->sectors_per_fat);
    
    uint8_t root_buf[512];
    ata_read_sector(root_dir_lba, (uint16_t*)root_buf);
    
    fat_dirent_t* entry = (fat_dirent_t*)root_buf;
    
    if (entry->name[0] != 0x00 && entry->name[0] != 0xE5) {
        kprint("First file found: ", 0xFFFFFF);

        for (int i = 0; i < 11; i++) {
            char path_char[2] = {(char)entry->name[i], '\0'};
            kprint(path_char, 0x00FF00);
            
            if (i == 7) kprint(".", 0x00FF00);
        }
        kprint("\n", 0xFFFFFF);
    } else {
        kprint("No valid files found in Root Directory.\n", 0xFF0000);
    }
}