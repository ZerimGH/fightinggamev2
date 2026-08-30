#ifndef PLAYER_H
#define PLAYER_H

#include "aabb.h"
#include "input.h"
#include <stdint.h>

#define PLAYER_RENDER_WIDTH (96 * 2)
#define PLAYER_RENDER_HEIGHT (63 * 2)

/* Unfortunately, player states cannot be in player_private.h
 * as they need to be referenced for GameFrame.players */
typedef enum {
    PLAYER_STATE_IDLE,
    PLAYER_STATE_WALK,
    PLAYER_STATE_PUNCH
} PlayerState;

typedef struct Player {
    AABB collision_box;
    int16_t x, y;
    PlayerState state;
    uint64_t frames;
    uint64_t anim_frame;
    int8_t facing;
    int connected;
} Player;

void player_init(Player *p, int idx, int tot);
void player_update(Player *p);
void player_apply_inputs(Player *p, Input inputs, Input linputs);
void player_state_change(Player *p, PlayerState new);

#endif /* PLAYER_H */
