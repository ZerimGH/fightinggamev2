#include "program_server_menu.h"
#include "button.h"
#include "client.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"
#include "server_info.h"
#include <stdio.h>

#define MAX_INFOS 64

static ServerInfo infos[MAX_INFOS];
static int num_infos = 0;

static Button buttons[MAX_INFOS];

int program_server_menu_enter(void) {
    if (!client_is_init()) {
        PERROR("Client not yet intialised\n");
        return 1;
    }
    num_infos = 0;
    return 0;
}

void program_server_menu_exit(void) { num_infos = 0; }

void program_server_menu_update(void) {
    client_update();

    num_infos = client_get_servers(infos, MAX_INFOS);

    if (IsKeyPressed(KEY_ESCAPE)) {
        client_deinit();
        program_state_change(PS_LAN_MENU);
        return;
    }

    for (int i = 0; i < num_infos; i++) {
        Button *button = &buttons[i];
        button->active = 1;

        snprintf(button->text,
            sizeof(button->text),
            "%s (%d/%d)",
            infos[i].name,
            infos[i].cur_players,
            infos[i].max_players);

        if (button_pressed(button, i, num_infos)) {
            if (client_join(infos[i].host, infos[i].port)) {
                PERROR("Failed to join server: %s\n", infos[i].name);
            } else {
                program_state_change(PS_WAITING);
                return;
            }
        }
    }
}

void program_server_menu_render(void) {
    ClearBackground(BLACK);

    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();

    /* Show escape to return in bottom left */
    const char *back_prompt = "Press escape to return";
    DrawText(back_prompt, 40, screen_h - 40, 18, GRAY);

    /* Display a message if no servers */
    if (num_infos == 0) {
        const char *no_servers = "No servers found";
        int ns_w               = MeasureText(no_servers, 20);
        DrawText(no_servers,
            (screen_w / 2.0f) - (ns_w / 2.0f),
            (float)GetScreenHeight() / 2.0f,
            20,
            GRAY);
        return;
    }

    /* Render buttons */
    for (int i = 0; i < num_infos; i++) {
        button_render(&buttons[i], i, num_infos);
    }
}
