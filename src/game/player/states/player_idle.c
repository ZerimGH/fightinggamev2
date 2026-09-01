#include "aabb.h"
#include "input.h"
#include "log.h"
#include "player.h"
#include "player_private.h"
#include <stdint.h>

#define PUSHBACK_SPEED 1

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

void player_idle_do_collisions(Player *p1, Player *p2) {
    AABB aabb1 = AABB_relative(&p1->collision_box, p1);
    AABB aabb2 = AABB_relative(&p2->collision_box, p2);
    if (AABB_check_AABB(&aabb1, &aabb2)) {
        if (p1->x >= p2->x) p1->x += PUSHBACK_SPEED;
        else p1->x -= PUSHBACK_SPEED;
    }
}
