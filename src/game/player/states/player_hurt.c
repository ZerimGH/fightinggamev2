#include "input.h"
#include "player.h"
#include "player_private.h"

void player_hurt_enter(Player *p) {
}

void player_hurt_exit(Player *p) {
}

void player_hurt_apply_inputs(Player *p, Input inputs, Input linputs) {}

void player_hurt_update(Player *p) {
    if (p->anim_frame >= 4) player_state_change(p, PLAYER_STATE_IDLE);
}

void player_hurt_do_collisions(Player *p1, Player *p2) { return; }
