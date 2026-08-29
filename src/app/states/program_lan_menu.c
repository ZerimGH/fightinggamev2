#include "program_lan_menu.h"
#include "button.h"
#include "client.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"
#include "server.h"

static Button buttons[] = {
    {"HOST A GAME", 1}, {"JOIN A GAME", 1}, {"BACK TO MENU", 1}};

#define BUTTON_COUNT (sizeof(buttons) / sizeof(buttons[0]))

int program_lan_menu_enter(void) { return 0; }

void program_lan_menu_exit(void) {}

void program_lan_menu_update(void) {
    for (int i = 0; i < (int)BUTTON_COUNT; i++) {
        if (button_pressed(&buttons[i], i, BUTTON_COUNT)) {
            switch (i) {
            case 0:
                /* Host a game */
                program_state_change(PROGRAM_STATE_HOST);
                break;
            case 1:
                /* Join a game */
                /* Create a client to scan for and connect to servers */
                if (client_init()) {
                    PERROR("Failed to initialise client\n");
                    return;
                }
                /* PROGRAM_STATE_SERVER_MENU will handle displaying a list of servers to join
                 * After a server has been joined it will go to PROGRAM_STATE_WAITING */
                program_state_change(PROGRAM_STATE_SERVER_MENU);
                break;
            case 2:
                /* Back to menu */
                program_state_change(PROGRAM_STATE_MENU);
                break;
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
