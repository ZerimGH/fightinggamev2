#include "program_gaming.h"
#include "game.h"
#include "input_manager.h"
#include "log.h"
#include "program.h"
#include "raylib/raylib.h"

#define DT (1.f / 60.f)

static double start_time = 0.0;
static double next_time  = 0.0;

static double esc_time = -1.0;

int program_gaming_enter(void) {
    if (!input_manager_is_init() || input_manager_over()) {
        PERROR("Input manager not ready\n");
        return 1;
    }

    if (game_init()) {
        PERROR("Failed to initialise game\n");
        return 1;
    }

    start_time = GetTime();
    next_time  = start_time + DT;

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
        if (input_manager_over()) {
            PINFO("Input manager stopped, returning to menu\n");
            program_state_change(PROGRAM_STATE_MENU); /* Will deinit everything */
            return;
        }
        next_time += DT;
    }

    if (IsKeyPressed(KEY_ESCAPE)) esc_time = GetTime();
    if (IsKeyReleased(KEY_ESCAPE)) esc_time = -1.0;

    if (esc_time > 0.0 && cur_time - esc_time >= 1.0) {
        esc_time = -1.0;
        PINFO("Returning to menu\n");
        program_state_change(PROGRAM_STATE_MENU); /* Will deinit everything */
        return;
    }
}

void program_gaming_render(void) {
    ClearBackground(BLACK);
    game_render();

    /* Show escape to return in bottom left */
    float screen_h          = (float)GetScreenHeight();
    const char *back_prompt = "Hold escape for 1 second to exit";
    if (esc_time > 0) DrawText(back_prompt, 40, screen_h - 40, 18, GRAY);
}
