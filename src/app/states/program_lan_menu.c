#include "program_lan_menu.h"
#include "button.h"
#include "client.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"
#include "server.h"
#include <stdlib.h>

static Button buttons[] = {{"HOST A GAME", 1}, {"JOIN A GAME", 1}, {"BACK TO MENU", 1}};

#define BUTTON_COUNT (sizeof(buttons) / sizeof(buttons[0]))

int program_lan_menu_enter(void) {
    return 0;
}

void program_lan_menu_exit(void) {
}

static char rand_char(void) {
    return rand() % ('Z' - 'A') + 1 + 'A';
}

void program_lan_menu_update(void) {
    for (int i = 0; i < (int)BUTTON_COUNT; i++) {
        if (button_pressed(&buttons[i], i, BUTTON_COUNT)) {
            switch (i) {
                case 0:
                    /* Host a game */
                    /* Create a server for the game */
                    /* Create random 5-char name for server (cba to do input boxes rn) */
                    char name[6];
                    for (int i = 0; i < 5; i++) name[i] = rand_char();
                    if (server_init(name, 2)) {
                        PERROR("Failed to initialise server\n");
                        return;
                    }
                    /* Create a client for the player and connect to server */
                    if (client_init()) {
                        PERROR("Failed to initialise client\n");
                        server_deinit();
                        return;
                    }
                    ServerInfo info = server_get_info();
                    if (client_join(info.host, info.port)) {
                        PERROR("Failed to connect client to server\n");
                        server_deinit();
                        client_deinit();
                        return;
                    }
                    /* PS_WAITING will handle updating the server and waiting 
                     * for the game to start */
                    /* TODO: Initialise input manager for LAN */
                    program_change_state(PS_WAITING);
                    break;
                case 1:
                    /* Join a game */
                    /* Create a client to scan for and connect to servers */
                    if (client_init()) {
                        PERROR("Failed to initialise client\n");
                        return;
                    }
                    /* PS_SERVER_MENU will handle displaying a list of servers to join
                     * After a server has been joined it will go to PS_WAITING */
                    program_change_state(PS_SERVER_MENU);
                    break;
                case 2:
                    /* Back to menu */
                    program_change_state(PS_MENU);
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
