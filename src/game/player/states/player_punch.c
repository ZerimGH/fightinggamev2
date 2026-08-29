#include "input.h"
#include "player.h"
#include "player_private.h"

void player_punch_enter(Player *p) {}

void player_punch_exit(Player *p) {}

void player_punch_apply_inputs(Player *p, Input inputs, Input linputs) {
    /*
    if (p->anim_frame >= 2) {
        if (inputs.fields.punch && !linputs.fields.punch) {
            player_state_change(p, PLAYER_STATE_PUNCH);
        }
    }
    */
}

void player_punch_update(Player *p) {
    if (p->anim_frame >= 3) player_state_change(p, PLAYER_STATE_IDLE);
}
