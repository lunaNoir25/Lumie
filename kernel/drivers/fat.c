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

void fat_format_name(const char* input, char* output) {
    for (int i = 0; i < 11; i++) output[i] = ' ';

    int i = 0;
    while (input[i] != '.' && input[i] != '\0' && i < 8) {
        if (input[i] >= 'a' && input[i] <= 'z') 
            output[i] = input[i] - 32;
        else 
            output[i] = input[i];
        i++;
    }

    const char* dot = input;
    while (*dot != '.' && *dot != '\0') dot++;

    if (*dot == '.') {
        dot++;
        for (int j = 0; j < 3 && dot[j] != '\0'; j++) {
            if (dot[j] >= 'a' && dot[j] <= 'z')
                output[8+j] = dot[j] - 32;
            else
                output[8+j] = dot[j];
        }
    }
}

int fat_name_match(const uint8_t* fat_name, const char* search_name) {
    for (int i = 0; i < 11; i++) {
        if (fat_name[i] != search_name[i]) return 0;
    }
    return 1;
}

void fat_list_files() {
    fat_bpb_t* bpb = (fat_bpb_t*)boot_sector;
    uint32_t root_lba = bpb->reserved_sectors + (bpb->fat_count * bpb->sectors_per_fat);
    
    uint8_t buf[512];
    ata_read_sector(root_lba, (uint16_t*)buf);
    fat_dirent_t* entries = (fat_dirent_t*)buf;

    for (int i = 0; i < 16; i++) {
        if (entries[i].name[0] == 0x00) break;
        if (entries[i].name[0] == 0xE5) continue;
        if (entries[i].attr & 0x08) continue;

        for (int j = 0; j < 8; j++) {
            char s[2] = {entries[i].name[j], '\0'};
            kprint(s, 0x00FF00);
        }
        kprint(".", 0x00FF00);
        for (int j = 8; j < 11; j++) {
            char s[2] = {entries[i].name[j], '\0'};
            kprint(s, 0x00FF00);
        }
        kprint("\n", 0xFFFFFF);
    }
}

uint16_t fat_find_file(const char* name, uint32_t* size_out) {
    fat_bpb_t* bpb = (fat_bpb_t*)boot_sector;
    uint32_t root_lba = bpb->reserved_sectors + (bpb->fat_count * bpb->sectors_per_fat);
    
    uint8_t buf[512];
    ata_read_sector(root_lba, (uint16_t*)buf);
    fat_dirent_t* entries = (fat_dirent_t*)buf;

    for (int i = 0; i < 16; i++) {
        if (fat_name_match(entries[i].name, name)) {
            *size_out = entries[i].file_size;
            return entries[i].first_cluster_lo;
        }
    }
    return 0;
}

void fat_read_file(uint16_t cluster, uint8_t* target_buffer) {
    fat_bpb_t* bpb = (fat_bpb_t*)boot_sector;
    uint32_t root_dir_sectors = ((bpb->root_entries * 32) + (bpb->bytes_per_sector - 1)) / bpb->bytes_per_sector;
    uint32_t first_data_sector = bpb->reserved_sectors + (bpb->fat_count * bpb->sectors_per_fat) + root_dir_sectors;
    
    uint32_t lba = ((cluster - 2) * bpb->sectors_per_cluster) + first_data_sector;
    ata_read_sector(lba, (uint16_t*)target_buffer);
}

void fat_init() {
    ata_read_sector(0, (uint16_t*)boot_sector);
    
    fat_bpb_t* bpb = (fat_bpb_t*)boot_sector;
    
    uint32_t root_dir_lba = bpb->reserved_sectors + (bpb->fat_count * bpb->sectors_per_fat);
    
    uint8_t root_buf[512];
    ata_read_sector(root_dir_lba, (uint16_t*)root_buf);
    
    fat_dirent_t* entry = (fat_dirent_t*)root_buf;
}