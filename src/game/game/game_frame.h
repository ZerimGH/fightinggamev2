#ifndef GAME_FRAME_H
#define GAME_FRAME_H

#define WORLD_W 800
#define WORLD_H 400

#define PLAYER_W (96 * 2)
#define PLAYER_H (63 * 2)

#define ANIM_FRAMES 12

#include "global.h"
#include "input.h"
#include <stdint.h>

typedef enum { PSIdle, PSWalk, PSPunch } PlayerState;

typedef struct {
    uint16_t x, y;

    PlayerState state;
    uint64_t frames;
    uint64_t anim_frame;
    int8_t facing;
    int connected;
} Player;

typedef struct {
    Player players[MAX_PLAYERS];
    Input inputs[MAX_PLAYERS];
    Input last_inputs[MAX_PLAYERS];
    uint64_t frame_no;
    uint8_t num_players;
} GameFrame;

void game_frame_init(GameFrame *gs);
void game_frame_apply_input(GameFrame *gs, uint8_t player_id, Input input);
void game_frame_finalise(GameFrame *gs);
void game_frame_render(GameFrame *gs);
uint64_t hash_game_frame(GameFrame *gs);

#endif /* GAME_FRAME_H */
