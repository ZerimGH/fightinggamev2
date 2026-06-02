#include "input_manager.h"
#include <stddef.h>
#include <stdint.h>
#include "global.h"
#include "input.h"
#include "log.h"
#include "raylib/raylib.h"

typedef enum { IM_LOCAL } InputManagerType;

typedef struct {
    /* Circular buffer of recent inputs */
    size_t head;
    size_t tail;
    Input inputs[MAX_ROLLBACK];
} InputBuf;

typedef struct {
    InputManagerType type;

    size_t num_players;

    InputBuf players[MAX_PLAYERS];

    size_t frame; /* Counter incremented on each tick() */
} InputManager;

static InputManager im = {0};
static int init = 0;

int input_manager_ready(void) {
    return init;
}

uint8_t input_manager_num_players(void) {
    if (!init) return 0;
    return im.num_players;
}

int input_manager_init(void) {
    if (init) {
        PERROR("Input manager already initialised\n");
        return 1;
    }

    /* Hard code values for now */
    im.type = IM_LOCAL;
    im.num_players = 2;
    /* Initialise player input buffers */
    for (uint8_t i = 0; i < im.num_players; i++) {
        im.players[i].head = 0;
        im.players[i].tail = 0;
    }
    im.frame = 0;

    init = 1;
    return 0;
}

void input_manager_deinit(void) {
    if (!init) {
        PERROR("(warn) deinit called when not yet initialised\n");
    }
    /* No clean up to be done yet */
    init = 0;
}

int input_manager_get_input(uint8_t player_id, Input *input) {
    if (!input) return 1;
    if (!init) {
        PERROR("(warn) get_input called when not yet initialised\n");
        return 1;
    }

    if (player_id >= im.num_players) return 1;
    InputBuf *player = &im.players[player_id];
    if (player->tail >= player->head) return 1; /* No new inputs */
    size_t idx = player->tail % MAX_ROLLBACK;
    *input = player->inputs[idx];
    player->tail++;
    return 0;
}

static int append(uint8_t player_id, TimedInput input) {
    if (player_id >= im.num_players) return 1;
    InputBuf *player = &im.players[player_id];
    if (input.frame != player->head) return 1; /* Match current head absolute frame */
    /* TODO: Handle this better, the player should be disconnected
     * and wait for the server to confirm disconnection or smth */
    if (player->head - player->tail >= MAX_ROLLBACK) return 1;       /* Too far behind */
    if ((player->head + 1) % MAX_ROLLBACK == player->tail) return 1; /* Too far ahead */
    size_t idx = player->head % MAX_ROLLBACK;
    player->inputs[idx] = input.input;
    player->head++;
    return 0;
}

void input_manager_tick(void) {
    if (!init) {
        PERROR("(warn) tick called when not yet initialised\n");
        return;
    }

    switch (im.type) {
        case IM_LOCAL:
            /* TODO: Abstract keypresses -> Input */
            uint64_t frame = im.frame;
            Input input_a;
            input_a.raw = 0;
            input_a.fields.left = IsKeyDown(KEY_A);
            input_a.fields.right = IsKeyDown(KEY_D);
            input_a.fields.punch = IsKeyDown(KEY_E);
            input_a.fields.kick = IsKeyDown(KEY_Q);

            Input input_b;
            input_b.raw = 0;
            input_b.fields.left = IsKeyDown(KEY_J);
            input_b.fields.right = IsKeyDown(KEY_L);
            input_b.fields.punch = IsKeyDown(KEY_O);
            input_b.fields.kick = IsKeyDown(KEY_U);

            TimedInput ti_a = {frame, input_a};
            TimedInput ti_b = {frame, input_b};

            if (append(0, ti_a) || append(1, ti_b)) {
                /* Should never fail, good to check ig */
                PERROR("Failed to append local inputs\n");
                return;
            }

            break;
        default: PERROR("(warn) unhandled input manager type %d\n", im.type); return;
    }
    im.frame++;
}
