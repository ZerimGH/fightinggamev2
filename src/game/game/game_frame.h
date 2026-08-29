#ifndef GAME_FRAME_H
#define GAME_FRAME_H

#include "global.h"
#include "input.h"
#include "player.h"
#include <stdint.h>

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
