#include "program.h"
#include "server.h"
#include "client.h"
#include "global.h"
#include "button.h"
#include "log.h"
#include "raylib/raylib.h"
#include <math.h>
#include <stddef.h>

static Button button = {"HOST", 1};

static char name[32] = {'0'};
static size_t name_len = 0;

static int num_players = 2;
static int show_cursor = 0;

int program_host_enter(void) {
    if (server_is_init()
            || client_is_init()) return 1;
    name[0] = '\0'; 
    name_len = 0;
    return 0;
}

void program_host_exit(void) {

}

static void type(char c) {
    if (name_len + 1 >= sizeof(name) / sizeof(char)) return;
    name[name_len++] = c;
    name[name_len] = '\0';
}

static void backspace(void) {
    if (name_len == 0) return;
    name_len--;
    name[name_len] = '\0';
}

static int validate(void) {
    if (name_len == 0) return 0;
    if (num_players >= MAX_PLAYERS || num_players < 2) return 0;
    return 1;
}

static int host(void) {
    if (server_init(name, num_players)) {
        PERROR("Failed to initialise server\n");
        return 1;
    }
    /* Create a client for the player and connect to server */
    if (client_init()) {
        PERROR("Failed to initialise client\n");
        server_deinit();
        return 1;
    }
    ServerInfo info = server_get_info();
    if (client_join(info.host, info.port)) {
        PERROR("Failed to connect client to server\n");
        server_deinit();
        client_deinit();
        return 1;
    }
    /* PS_WAITING will handle updating the server and waiting 
     * for the game to start */
    program_change_state(PS_WAITING);
    return 0;
}

void program_host_update(void) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        program_change_state(PS_LAN_MENU);
        return;
    }

    show_cursor = ((int)floor(GetTime() * 2.5) % 2);

    int char_pressed;
    while ((char_pressed = GetCharPressed())) {
        /* The first 128 unicode characters are equivalent to ascii */
        if (char_pressed > 128) continue;
        char c = (char)char_pressed;
        type(c);
    }

    if (IsKeyPressed(KEY_BACKSPACE)) backspace();

    /* Write to name based on keyboard input */ 
    /* Update num_players based on up / down pressed */
    if (IsKeyPressed(KEY_UP)) {
        if (num_players < MAX_PLAYERS) num_players++;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        if (num_players > 2) num_players--;
    }

    button_set_active(&button, validate());

    if (button_pressed(&button, 0, 1)) {
        if (!host()) return;
    }
}

void program_host_render(void) {
    ClearBackground(BLACK);
    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();
    float cx = screen_w / 2.f;
    float cy = 50;

    /* Show escape to return in bottom left */
    const char *back_prompt = "Press escape to return";
    DrawText(back_prompt, 40, screen_h - 40, 18, GRAY);

    const char *name_text = TextFormat("Name (type): %s", name);
    int font_size = 40;
    int text_width = MeasureText(name_text, font_size);
    int text_height = font_size;
    float text_x = cx - (text_width / 2.f);
    float text_y = cy - (text_height / 2.f);
    DrawText(name_text, text_x, text_y, font_size, WHITE);

    if (show_cursor) {
        float cursor_w = 4.f;
        float cursor_h = (float)font_size;
        float cursor_x = text_x + text_width + 4.f;
        DrawRectangle(cursor_x, text_y, cursor_w, cursor_h, WHITE);
    }

    const char *players_text = TextFormat("Players (press up/down): %d",
            (int)num_players);

    int players_text_width = MeasureText(players_text, font_size);
    float players_cy = cy + (text_height / 2.f) + 20.f;
    DrawText(players_text, cx - (players_text_width / 2.f), 
            players_cy, font_size, WHITE);

    button_render(&button, 0, 1);
}
