#include "player.h"
#include "aabb.h"
#include "global.h"
#include "input.h"
#include "player_idle.h"
#include "player_private.h"
#include "player_punch.h"
#include "player_walk.h"
#include "world.h"
#include <stdint.h>

#define ANIM_FRAMES 12

#define DO_MAGIC                                                                 \
    X(PLAYER_STATE_IDLE, player_idle)                                            \
    X(PLAYER_STATE_WALK, player_walk)                                            \
    X(PLAYER_STATE_PUNCH, player_punch)

void player_init(Player *p, int idx, int tot) {
    int16_t spacing  = (WORLD_WIDTH - PLAYER_RENDER_WIDTH) / (tot - 1);
    p->x             = spacing * idx;
    p->y             = 0;
    p->collision_box = (AABB){PLAYER_RENDER_WIDTH / 2 - PLAYER_RENDER_WIDTH / 10,
        0,
        PLAYER_RENDER_WIDTH / 6,
        PLAYER_RENDER_HEIGHT * 2 / 3};
    p->state         = PLAYER_STATE_IDLE;
    p->frames        = 0;
    p->anim_frame    = 0;
    p->facing        = 1;
    p->connected     = 1;
}

static void player_state_enter(Player *p, PlayerState new) {
    p->state      = new;
    p->frames     = 0;
    p->anim_frame = 0;

#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_enter(p); break;

    switch (new) { DO_MAGIC }
#undef X
}

static void player_state_exit(Player *p) {
    p->frames = 0;
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_exit(p); break;

    switch (p->state) { DO_MAGIC }
#undef X
}

void player_update(Player *p) {
    p->frames++;
    p->anim_frame = p->frames / ANIM_FRAMES;

#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_update(p); break;

    switch (p->state) { DO_MAGIC }

#undef X
}

void player_apply_inputs(Player *p, Input inputs, Input linputs) {
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_apply_inputs(p, inputs, linputs); break;
    switch (p->state) { DO_MAGIC }
#undef X
}

void player_state_change(Player *p, PlayerState new) {
    if (p->state == new) { return; }
    player_state_exit(p);
    player_state_enter(p, new);
}

void player_do_collisions(Player *p1, Player *p2) {
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_do_collisions(p1, p2); break;
    switch (p1->state) { DO_MAGIC }
#undef X

    AABB aabb = AABB_relative(&p1->collision_box, p1);
    if (aabb.x < 0) p1->x -= aabb.x;
    if (aabb.x + aabb.w > WORLD_WIDTH) p1->x -= ((aabb.x + aabb.w) - WORLD_WIDTH);
}
