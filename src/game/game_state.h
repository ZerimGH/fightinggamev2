#ifndef GAME_STATE_H
#define GAME_STATE_H

#define WORLD_W     800
#define WORLD_H     400

#define PLAYER_W    (96 * 2)
#define PLAYER_H    (63 * 2)

#define ANIM_FRAMES 12

#include <stdint.h>
#include "global.h"
#include "input.h"

typedef enum { PSIdle, PSWalk, PSPunch } PlayerState;

typedef struct {
    uint16_t x, y;

    PlayerState state;
    uint64_t frames;
    uint64_t anim_frame;
    int8_t facing;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    Input inputs[MAX_PLAYERS];
    Input last_inputs[MAX_PLAYERS];
    uint64_t frame_no;
} GameState;

void game_state_init(GameState *gs);
void game_state_apply_input(GameState *gs, uint8_t player_id, Input input);
void game_state_finalise(GameState *gs);
void game_state_render(GameState *gs);
uint64_t hash_game_state(GameState *gs);

#endif /* GAME_STATE_H */
