#include "input.h"
#include "player.h"
#include "player_private.h"

void player_idle_enter(Player *p) {}

void player_idle_exit(Player *p) {}

void player_idle_update(Player *p) {}

void player_idle_apply_inputs(Player *p, Input inputs, Input linputs) {
    int left  = inputs.fields.left;
    int right = inputs.fields.right;

    if ((left || right) && !(left && right)) {
        player_state_change(p, PLAYER_STATE_WALK);
    }

    if (inputs.fields.punch && !linputs.fields.punch) {
        player_state_change(p, PLAYER_STATE_PUNCH);
    }
}
