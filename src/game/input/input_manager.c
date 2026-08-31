#include "input_manager.h"
#include "client.h"
#include "global.h"
#include "input.h"
#include "log.h"
#include "raylib/raylib.h"
#include "server.h"
#include <stddef.h>
#include <stdint.h>

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
static int init        = 0;
static int over        = 1;
static float init_time = 0;

static void input_manager_finish(void) {
    if (!init) return;
    over = 1;
}

int input_manager_is_init(void) { return init; }

uint8_t input_manager_num_players(void) {
    if (!init) return 0;
    return im.num_players;
}

int input_manager_init(InputManagerType type) {
    if (init) {
        PERROR("Input manager already initialised\n");
        return 1;
    }

    switch (type) {
    case IM_LOCAL: im.num_players = 2; break;
    case IM_LAN:
        if (!client_is_init() || !client_started()) {
            PERROR("Input manager initialised with type IM_LAN, but client"
                   " is not ready\n");
            return 1;
        }
        im.num_players = client_get_num_players();
        break;
    }
    im.type = type;

    /* Initialise player input buffers */
    for (uint8_t i = 0; i < im.num_players; i++) {
        im.players[i].head = 0;
        im.players[i].tail = 0;
    }
    im.frame = 0;

    init = 1;
    over = 0;
    init_time = GetTime();
    return 0;
}

void input_manager_deinit(void) {
    if (!init) { PERROR("(warn) deinit called when not yet initialised\n"); }
    if (im.type == IM_LAN) {
        if (client_is_init()) client_deinit();
        if (server_is_init()) server_deinit();
    }
    init = 0;
    over = 1;
}

int input_manager_get_input(uint8_t player_id, Input *input) {
    if (over) return 1;
    if (!input) return 1;
    if (!init) {
        PERROR("(warn) get_input called when not yet initialised\n");
        return 1;
    }

    if (player_id >= im.num_players) return 1;
    InputBuf *player = &im.players[player_id];
    if (player->tail >= player->head) return 1; /* No new inputs */
    size_t idx = player->tail % MAX_ROLLBACK;
    *input     = player->inputs[idx];
    player->tail++;
    return 0;
}

static int append(uint8_t player_id, TimedInput input) {
    if (player_id >= im.num_players) return 1;
    InputBuf *player = &im.players[player_id];
    if (input.frame != player->head)
        return 1; /* Make sure the frame matches the current head*/
    if (player->head - player->tail >= MAX_ROLLBACK)
        return 1; /* Too far behind */
    if ((player->head + 1) % MAX_ROLLBACK == player->tail)
        return 1; /* Too far ahead */
    size_t idx          = player->head % MAX_ROLLBACK;
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
    case IM_LOCAL: {
        /* TODO: Abstract keypresses -> Input */
        uint64_t frame = im.frame;
        Input input_a;
        input_a.raw          = 0;
        input_a.fields.left  = IsKeyDown(KEY_A);
        input_a.fields.right = IsKeyDown(KEY_D);
        input_a.fields.punch = IsKeyDown(KEY_E);
        input_a.fields.kick  = IsKeyDown(KEY_Q);

        Input input_b;
        input_b.raw          = 0;
        input_b.fields.left  = IsKeyDown(KEY_J);
        input_b.fields.right = IsKeyDown(KEY_L);
        input_b.fields.punch = IsKeyDown(KEY_O);
        input_b.fields.kick  = IsKeyDown(KEY_U);

        TimedInput ti_a = {frame, input_a};
        TimedInput ti_b = {frame, input_b};

        if (append(0, ti_a) || append(1, ti_b)) {
            /* Should never fail, good to check ig */
            PERROR("Failed to append local inputs\n");
            input_manager_finish();
            return;
        }

        break;
    }
    case IM_LAN: {
        /* Update client */
        client_update();

        /* Don't simulate until delay is over */
        if (GetTime() <= init_time + (float)client_get_delay()) return;
        /* Get local player's inputs */
        uint64_t frame = im.frame;
        Input input_a;
        input_a.raw          = 0;
        input_a.fields.left  = IsKeyDown(KEY_A);
        input_a.fields.right = IsKeyDown(KEY_D);
        input_a.fields.punch = IsKeyDown(KEY_E);
        input_a.fields.kick  = IsKeyDown(KEY_Q);
        TimedInput ti_a      = {frame, input_a};
        /* Send to server */
        if (client_send_input(ti_a)) {
            PERROR("Failed to send local inputs to server\n");
            input_manager_finish();
            return;
        }

        /* Update all players' inputs from client */
        for (uint8_t i = 0; i < im.num_players; i++) {
            if (!client_is_connected(i, im.frame)) {
                InputBuf *player = &im.players[i];
                /* Write no input for disconnected players */
                while (player->head <= im.frame) {
                    TimedInput p_ti = {
                        .frame = player->head, .input = {.raw = 0}};
                    if (append(i, p_ti)) {
                        PERROR("Failed to append no input for"
                               "disconnected player %d\n",
                            (int)i);
                        input_manager_finish();
                        return;
                    }
                }
            }
            InputBuf *player = &im.players[i];
            /* Check for new inputs */
            uint64_t p_frame = player->head;
            Input p_input;
            while (!client_get_input(i, p_frame, &p_input)) {
                TimedInput p_ti;
                p_ti.frame = p_frame;
                p_ti.input = p_input;
                if (append(i, p_ti)) {
                    PERROR("Failed to append inputs from client\n");
                    input_manager_finish();
                    return;
                }
                p_frame++;
            }
        }

        /* Update server if running here */
        if (server_is_init()) {
            server_update();
            /*
            if (server_count_clients() < 2) {
                input_manager_finish();
                return;
            }
            */
        }
        break;
    }
    default:
        PERROR("Unhandled input manager type %d\n", im.type);
        input_manager_finish();
        return;
    }
    im.frame++;
}

int input_manager_disconnected(uint8_t player_id) {
    if (!init) return 1;
    switch (im.type) {
    case IM_LOCAL: return 0;
    case IM_LAN:   return !client_is_connected(player_id, im.frame);
    }

    return 1;
}

int input_manager_over() {
    if (!init) return 1;
    return over;
}
