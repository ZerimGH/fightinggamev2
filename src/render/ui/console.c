#include "console.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "raylib/raylib.h"

#define CONSOLE_SIZE     512
#define CONSOLE_MSG_TIME 5.0f

char chars[CONSOLE_SIZE];
float times[CONSOLE_SIZE];
int head = 0;
int tail = 0;

void console_printf(const char *fmt, ...) {
    char buffer[CONSOLE_SIZE];

    va_list args;
    va_start(args, fmt);
    int length = vsnprintf(buffer, CONSOLE_SIZE, fmt, args);
    va_end(args);

    float time = (float)GetTime();

    for (int i = 0; i < length; i++) {
        chars[head] = buffer[i];
        times[head] = time;

        head = (head + 1) % CONSOLE_SIZE;

        if (head == tail) {
            tail = (tail + 1) % CONSOLE_SIZE;
        }
    }
}

void console_update(void) {
    float time = (float)GetTime();

    while (tail != head && (time - times[tail]) > CONSOLE_MSG_TIME) {
        tail = (tail + 1) % CONSOLE_SIZE;
    }
}

void console_render(void) {
    if (tail == head) {
        return;
    }
    int screen_h = GetScreenHeight();

    float font_size = (float)screen_h / 40.0f;
    if (font_size < 10.0f) {
        font_size = 10.0f;
    }

    static char buf[CONSOLE_SIZE + 1];
    int count = 0;
    int current = tail;

    while (current != head) {
        buf[count++] = chars[current];
        current = (current + 1) % CONSOLE_SIZE;
    }
    buf[count] = '\0';

    Vector2 position = {10.0f, 10.0f};
    DrawTextEx(GetFontDefault(), buf, position, font_size, 1.0f, WHITE);
}
