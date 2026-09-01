#include "input.h"
#include "player.h"
#include "player_private.h"
#include "world.h"

#define PLAYER_SPEED 4

void player_walk_enter(Player *p) {}

void player_walk_exit(Player *p) {}

void player_walk_apply_inputs(Player *p, Input inputs, Input linputs) {
    int left  = inputs.fields.left;
    int right = inputs.fields.right;

    if ((left && right) || !(left || right)) {
        player_state_change(p, PLAYER_STATE_IDLE);
        return;
    }

    if (left && !right) { p->facing = -1; }
    if (right && !left) { p->facing = 1; }
    if (inputs.fields.punch && !linputs.fields.punch) {
        player_state_change(p, PLAYER_STATE_PUNCH);
    }
}

void player_walk_update(Player *p) {
    if (p->facing < 0) p->x -= PLAYER_SPEED;
    else if (p->facing > 0) p->x += PLAYER_SPEED;
}

void player_walk_do_collisions(Player *p1, Player *p2) { return; }
