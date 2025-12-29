#include "tar.h"
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