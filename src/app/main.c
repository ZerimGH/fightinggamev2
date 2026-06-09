#include "log.h"
#include "program.h"
#include "raylib/raylib.h"
#include "game_state_renderer.h"

/* Enet forward declarations (can't include both raylib and enet due to windows.h) */
extern int enet_initialize(void);
extern void enet_deinitialize(void);

int quit = 0;

int init(void) {
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

    if (game_state_renderer_init()) {
        enet_deinitialize();
        CloseWindow();
    }

    if (program_init()) {
        game_state_renderer_deinit();
        enet_deinitialize();
        CloseWindow();
        return 1;
    }

    return 0;
}

void deinit(void) {
    game_state_renderer_deinit();
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
