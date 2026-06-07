#include "program_menu.h"
#include <stdlib.h>
#include "button.h"
#include "game.h"
#include "input_manager.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"

extern int quit;

static Button buttons[] = {{"LAN", 1}, {"LOCAL (2P)", 1}, {"QUIT", 1}};

#define BUTTON_COUNT (sizeof(buttons) / sizeof(buttons[0]))

int program_menu_enter(void) {
    return 0;
}

void program_menu_exit(void) {
}

void program_menu_update(void) {
    for (int i = 0; i < (int)BUTTON_COUNT; i++) {
        if (button_pressed(&buttons[i], i, BUTTON_COUNT)) {
            switch (i) {
                case 0:
                    /* LAN */
                    program_change_state(PS_LAN_MENU);
                    break;
                case 1:
                    /* Local game*/
                    if (input_manager_init(IM_LOCAL)) {
                        PERROR("Failed to initialise input manager\n");
                        break;
                    }
                    if (program_change_state(PS_GAMING)) {
                        input_manager_deinit();
                        PERROR("Failed to change to gaming state\n");
                    }
                    break;
                case 2: quit = 1; break;
            }
        }
    }
}

void program_menu_render(void) {
    ClearBackground(BLACK);
    for (int i = 0; i < (int)BUTTON_COUNT; i++) {
        button_render(&buttons[i], i, BUTTON_COUNT);
    }
}
