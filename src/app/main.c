#include "assets.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"
#include "sprite_sheet_manager.h"
#include "sprite_sheets.h"
#include <stdlib.h>
#include <time.h>

/* Enet forward declarations (can't include both raylib and enet due to windows.h) */
extern int enet_initialize(void);
extern void enet_deinitialize(void);

int quit = 0;

int init(void) {
    srand(time(NULL));
    SetTraceLogLevel(LOG_ERROR); /* Hide info messages */
    /* Set window to be resizable and to run even when minimised */
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(1440, 720, "game");

    /* Check for failure creating window */
    if (!IsWindowReady()) return 1;
    SetExitKey(0); /* Unbind exit key so ESC doesn't quit the program */
    if (enet_initialize()) {
        CloseWindow();
        return 1;
    }

    /* Load assets */
    if (ssm_init()) return 1;
    player_idle_sprite = ssm_load(".png", idle_png, sizeof(idle_png), 1, 4);
    player_walk_sprite = ssm_load(".png", walk_png, sizeof(walk_png), 1, 3);
    player_punch_sprite = ssm_load(".png", punch_png, sizeof(punch_png), 1, 3);
    if (player_walk_sprite == -1 || player_idle_sprite == -1 || player_punch_sprite == -1) {
        ssm_deinit();
        enet_deinitialize();
        CloseWindow();
        return 1;
    }

    if (program_init()) {
        ssm_deinit();
        enet_deinitialize();
        CloseWindow();
        return 1;
    }

    return 0;
}

void deinit(void) {
    ssm_deinit();
    program_deinit();
    enet_deinitialize();
    CloseWindow();
}

int main(void) {
    if (init()) {
        PERROR("Failed to initialise\n");
        return 1;
    }

    while (!WindowShouldClose() && !quit) {
        program_update();
        console_update();
        BeginDrawing();
        program_render();
        console_render();
        EndDrawing();
    }

    deinit();
    return 0;
}
