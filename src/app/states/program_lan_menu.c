#include "program_lan_menu.h"
#include "button.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"

static Button buttons[] = {{"HOST A GAME", 1}, {"JOIN A GAME", 1}, {"BACK TO MENU", 1}};

#define BUTTON_COUNT (sizeof(buttons) / sizeof(buttons[0]))

int program_lan_menu_enter(void) {
    return 0;
}

void program_lan_menu_exit(void) {
}

void program_lan_menu_update(void) {
    for (int i = 0; i < (int)BUTTON_COUNT; i++) {
        if (button_pressed(&buttons[i], i, BUTTON_COUNT)) {
            switch (i) {
                case 0: PERROR("TODO\n"); break;
                case 1: PERROR("TODO\n"); break;
                case 2: program_change_state(PS_MENU); break;
            }
        }
    }
}

void program_lan_menu_render(void) {
    ClearBackground(BLACK);
    for (int i = 0; i < (int)BUTTON_COUNT; i++) {
        button_render(&buttons[i], i, BUTTON_COUNT);
    }
}
