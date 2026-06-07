#include "program_gaming.h"
#include "game.h"
#include "input_manager.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"

#define DT (1.f / 60.f)

static double start_time = 0.0;
static double next_time = 0.0;

int program_gaming_enter(void) {
    if (!input_manager_is_init() || !input_manager_is_ok()) {
        PERROR("Input manager not ready\n");
        return 1;
    }

    if (game_init()) {
        PERROR("Failed to initialise game\n");
        return 1;
    }

    start_time = GetTime();
    next_time = start_time + DT;

    return 0;
}

void program_gaming_exit(void) {
    game_deinit();
    input_manager_deinit();
}

void program_gaming_update(void) {
    double cur_time = GetTime();
    if (cur_time >= next_time) {
        game_tick();
        if (!input_manager_is_ok()) {
            PERROR("Input manager not ok, returning to menu\n");
            program_change_state(PS_MENU); /* Will deinit everything */
            return;
        }
        next_time += DT;
    }
}

void program_gaming_render(void) {
    ClearBackground(BLACK);
    game_render();
}
