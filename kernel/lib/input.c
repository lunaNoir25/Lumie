#include "input.h"
#include "../drivers/keyboard.h"
#include "../drivers/screen.h"
#include "string.h"

void readline(char* buffer, size_t max_len) {
    size_t i = 0;

    while (1) {
        keyboard_handler();

        char c = get_last_char();

        if (c == '\0') {
            __asm__("pause");
            continue;
        }

        if (c == '\n') {
            buffer[i] = '\0';
            kprint("\n", 0xFFFFFF);
            break;
        }

        if (c == '\b') {
            if (i > 0) {
                i--;
                backspace();
            }
            continue;
        }

        if (i < max_len - 1) {
            buffer[i++] = c;
            char str[2] = {c, '\0'};
            kprint(str, 0xFFFFFF);
        }
    }
}