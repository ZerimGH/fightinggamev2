#include "game.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "game_state.h"
#include "global.h"
#include "input_manager.h"
#include "log.h"

typedef struct {
    /* Stores inputs of all players on a specific frame */
    /* Inputs can be predicted */
    Input player_inputs[MAX_PLAYERS];
    int predicted[MAX_PLAYERS];
} StateInputs;

typedef struct {
    GameState states[MAX_ROLLBACK];
    StateInputs inputs[MAX_ROLLBACK];
    size_t latest_inputs[MAX_PLAYERS];
    size_t head;
} Game;

static Game game = {0};
static int init = 0;

static void advance_state(GameState *state, const StateInputs *inputs) {
    uint8_t num_players = input_manager_num_players();

    for (uint8_t i = 0; i < num_players; i++) {
        game_state_apply_input(state, i, inputs->player_inputs[i]);
    }

    game_state_finalise(state);
}

int game_init(void) {
    if (init) {
        PERROR("init() called when game already initialised\n");
        return 1;
    }

    if (!input_manager_ready()) {
        PERROR("init() called before input manager initialised\n");
        return 1;
    }

    /* Initialise game struct */
    for (size_t i = 0; i < MAX_PLAYERS; i++) {
        game.latest_inputs[i] = 0;
    }
    game.head = 0;

    /* Set up initial game state */
    size_t start_idx = game.head % MAX_ROLLBACK;
    memset(&game.states[start_idx], 0, sizeof(GameState));
    size_t num_players = input_manager_num_players();
    for (size_t i = 0; i < num_players; i++) {
        game.states[start_idx].players[i].x = WORLD_W / (num_players + 1) * i;
        game.states[start_idx].players[i].y = 0;
    }

    init = 1;
    return 0;
}

void game_deinit(void) {
    if (!init) {
        /* IDK what to do here */
        PERROR("(warn) deinit() called before game initialised\n");
        return;
    }
    /* Not sure what to do here, maybe deinit input manager?
     * idk ill decide where its deinitialised later */
    init = 0;
}

void game_tick(void) {
    if (!init) {
        PERROR("(warn) tick() called before game initialised\n");
        return;
    }

    input_manager_tick();

    size_t current_idx = game.head % MAX_ROLLBACK;
    size_t next_idx = (game.head + 1) % MAX_ROLLBACK;

    uint8_t num_players = input_manager_num_players();
    int rollback = 0;
    size_t rollback_frame = game.head;

    /* Check if players have new inputs */
    for (uint8_t i = 0; i < num_players; i++) {
        Input new_input;
        while (input_manager_get_input(i, &new_input) == 0) {
            size_t input_frame = game.latest_inputs[i] + 1;
            size_t input_idx = input_frame % MAX_ROLLBACK;

            /* Check if prediction was wrong */
            if (input_frame < game.head && game.inputs[input_idx].predicted[i]) {
                if (game.inputs[input_idx].player_inputs[i].raw != new_input.raw) {
                    rollback = 1;
                    if (input_frame < rollback_frame) {
                        rollback_frame = input_frame;
                    }
                }
            }

            game.inputs[input_idx].player_inputs[i] = new_input;
            game.inputs[input_idx].predicted[i] = 0;
            game.latest_inputs[i] = input_frame;
        }
    }

    if (rollback) {
        for (size_t i = rollback_frame; i < game.head; i++) {
            size_t cur = i % MAX_ROLLBACK;
            size_t next = (i + 1) % MAX_ROLLBACK;

            memcpy(&game.states[next], &game.states[cur], sizeof(GameState));
            advance_state(&game.states[next], &game.inputs[cur]);
        }
    }

    /* Predict missing inputs */
    for (uint8_t i = 0; i < num_players; i++) {
        if (game.latest_inputs[i] < game.head) {
            size_t latest_idx = game.latest_inputs[i] % MAX_ROLLBACK;

            game.inputs[current_idx].player_inputs[i] = game.inputs[latest_idx].player_inputs[i];
            game.inputs[current_idx].predicted[i] = 1;
        }
    }

    memcpy(&game.states[next_idx], &game.states[current_idx], sizeof(GameState));
    advance_state(&game.states[next_idx], &game.inputs[current_idx]);

    game.head++;
}

void game_render(void) {
    if (!init) {
        return;
    }

    /* Render current game state */
    size_t current_idx = game.head % MAX_ROLLBACK;
    game_state_render(&game.states[current_idx]);
}
