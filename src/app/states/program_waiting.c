#include "program_waiting.h"
#include "client.h"
#include "input_manager.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"
#include "server.h"
#include "server_info.h"

int program_waiting_enter(void) {
    if (!client_is_init()) return 1;
    return 0;
}

void program_waiting_exit(void) { /* Nothing to be done */ }

void program_waiting_update(void) {
    if (IsKeyPressed(KEY_ESCAPE)) { client_disconnect(); }

    if (client_disconnected()) {
        client_deinit();
        if (server_is_init()) server_deinit();
        PERROR("Disconnected from server, returning to LAN menu\n");
        program_state_change(PROGRAM_STATE_LAN_MENU);
        return;
    }

    if (client_started()) {
        if (input_manager_init(IM_LAN)) {
            PERROR("Failed to initialise input manager\n");
            client_deinit();
            program_state_change(PROGRAM_STATE_LAN_MENU);
        } else {
            program_state_change(PROGRAM_STATE_GAMING);
        }
        return;
    }

    if (server_is_init()) { server_update(); }
    client_update();
}

void program_waiting_render(void) {
    ClearBackground(BLACK);

    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();
    float cx       = screen_w / 2.f;
    float cy       = screen_h / 2.f;

    /* Show escape to return in bottom left */
    const char *back_prompt = "Press escape to disconnect";
    DrawText(back_prompt, 40, screen_h - 40, 18, GRAY);

    const char *text = client_connection_confirmed() ? "Waiting for players..."
                                                     : "Connecting...";
    int font_size    = 40;
    int text_width   = MeasureText(text, font_size);
    int text_height  = font_size;
    DrawText(text,
        cx - (text_width / 2.f),
        cy - (text_height / 2.f),
        font_size,
        WHITE);

    /* Show server information if running here */
    if (server_is_init()) {
        ServerInfo info = server_get_info();

        const char *server_text = TextFormat(
            "%s (%d/%d)", info.name, info.cur_players, info.max_players);

        int server_text_width = MeasureText(server_text, font_size / 3 * 2);
        float server_cy       = cy + (text_height / 2.f) + 20.f;
        DrawText(server_text,
            cx - (server_text_width / 2.f),
            server_cy,
            font_size / 3 * 2,
            LIGHTGRAY);
    }
}
