#include "game_state.h"
#include <stddef.h>
#include <stdint.h>
#include "global.h"
#include "input.h"
#include "input_manager.h"
#include "raylib/raylib.h"
#include "sprite_sheet_manager.h"
#include "sprite_sheets.h"

#define PLAYER_SPEED 2

static void player_idle_enter(Player *p);
static void player_idle_exit(Player *p);
static void player_idle_update(Player *p);
static void player_idle_apply_inputs(Player *p, Input inputs, Input linputs);

static void player_walk_enter(Player *p);
static void player_walk_exit(Player *p);
static void player_walk_update(Player *p);
static void player_walk_apply_inputs(Player *p, Input inputs, Input linputs);

static void player_punch_enter(Player *p);
static void player_punch_exit(Player *p);
static void player_punch_update(Player *p);
static void player_punch_apply_inputs(Player *p, Input inputs, Input linputs);

static void player_state_enter(Player *p, PlayerState new) {
    p->state = new;
    p->frames = 0;
    p->anim_frame = 0;

    switch (new) {
        case PSIdle: player_idle_enter(p); break;
        case PSWalk: player_walk_enter(p); break;
        case PSPunch: player_punch_enter(p); break;
    }
}

static void player_state_exit(Player *p) {
    p->frames = 0;
    switch (p->state) {
        case PSIdle: player_idle_exit(p); break;
        case PSWalk: player_walk_exit(p); break;
        case PSPunch: player_punch_exit(p); break;
    }
}

static void player_state_update(Player *p) {
    p->frames++;
    p->anim_frame = p->frames / ANIM_FRAMES;

    switch (p->state) {
        case PSIdle: player_idle_update(p); break;
        case PSWalk: player_walk_update(p); break;
        case PSPunch: player_punch_update(p); break;
    }
}

static void player_state_apply_inputs(Player *p, Input inputs, Input linputs) {
    switch (p->state) {
        case PSIdle: player_idle_apply_inputs(p, inputs, linputs); break;
        case PSWalk: player_walk_apply_inputs(p, inputs, linputs); break;
        case PSPunch: player_punch_apply_inputs(p, inputs, linputs); break;
    }
}

static void player_state_change(Player *p, PlayerState new) {
    if (p->state == new) {
        return;
    }
    player_state_exit(p);
    player_state_enter(p, new);
}

void game_state_init(GameState *gs) {
    if (!gs) {
        return;
    }

    gs->frame_no = 0;
    size_t players = input_manager_num_players();

    uint16_t spacing = (WORLD_W - PLAYER_W) / (players - 1);

    for (unsigned int i = 0; i < players; i++) {
        Player *p = &gs->players[i];

        p->x = spacing * i;
        p->y = 0;
        p->state = PSIdle;
        p->frames = 0;
        p->facing = 1;

        gs->inputs[i].raw = 0;
        gs->last_inputs[i].raw = 0;
    }
}

void game_state_apply_input(GameState *gs, uint8_t player_id, Input input) {
    if (!gs) {
        return;
    }
    size_t players = input_manager_num_players();
    if (player_id >= players) {
        return;
    }

    gs->last_inputs[player_id] = gs->inputs[player_id];
    gs->inputs[player_id] = input;

    player_state_update(&gs->players[player_id]);
    player_state_apply_inputs(&gs->players[player_id], gs->inputs[player_id], gs->last_inputs[player_id]);
}

void game_state_finalise(GameState *gs) {
    if (!gs) {
        return;
    }

    size_t players = input_manager_num_players();
    for (uint8_t i = 0; i < players; i++) {
        Player *p = &gs->players[i];

        if (p->state == PSWalk) {
            player_walk_update(p);
        } else if (p->state == PSPunch) {
            player_punch_update(p);
        } else {
            player_idle_update(p);
        }
    }

    gs->frame_no++;
}

static void player_idle_enter(Player *p) {
}

static void player_idle_exit(Player *p) {
}

static void player_idle_update(Player *p) {
}

static void player_idle_apply_inputs(Player *p, Input inputs, Input linputs) {
    int left = inputs.fields.left;
    int right = inputs.fields.right;

    if ((left || right) && !(left && right)) {
        player_state_change(p, PSWalk);
    }

    if (inputs.fields.punch && !linputs.fields.punch) {
        player_state_change(p, PSPunch);
    }
}

static void player_walk_enter(Player *p) {
}

static void player_walk_exit(Player *p) {
}

static void player_walk_apply_inputs(Player *p, Input inputs, Input linputs) {
    int left = inputs.fields.left;
    int right = inputs.fields.right;

    if ((left && right) || !(left || right)) {
        player_state_change(p, PSIdle);
        return;
    }

    if (left && !right) {
        p->facing = -1;
    }
    if (right && !left) {
        p->facing = 1;
    }
    if (inputs.fields.punch && !linputs.fields.punch) {
        player_state_change(p, PSPunch);
    }
}

static void player_walk_update(Player *p) {
    if (p->facing < 0) {
        if (p->x >= PLAYER_SPEED) {
            p->x -= PLAYER_SPEED;
        }
    } else if (p->facing > 0) {
        if (p->x + PLAYER_W + PLAYER_SPEED <= WORLD_W) {
            p->x += PLAYER_SPEED;
        }
    }
}

static void player_punch_enter(Player *p) {
}

static void player_punch_exit(Player *p) {
}

static void player_punch_apply_inputs(Player *p, Input inputs, Input linputs) {
    if (p->anim_frame >= 2) {
        if (inputs.fields.punch && !linputs.fields.punch) {
            player_state_change(p, PSPunch);
        }
    }
}

static void player_punch_update(Player *p) {
    if (p->anim_frame >= 3) {
        player_state_change(p, PSIdle);
    }
}

static uint64_t hash(void *data, size_t size) {
    uint64_t hash = 5381;
    int c;

    for (size_t i = 0; i < size; i++) {
        c = ((char *)data)[i];
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

uint64_t hash_game_state(GameState *gs) {
    if (!gs) {
        return 0;
    }
    size_t players = input_manager_num_players();
    return hash((void *)&gs->players[0], sizeof(Player) * players);
}

/* TODO: Improve rendering
 * It's not broken but it's so ugly I need to write a whole proper library
 * for rendering I think */
void game_state_render(GameState *gs) {
    if (!gs) {
        return;
    }

    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();

    float w_aspect = (float)WORLD_W / (float)WORLD_H;
    float s_aspect = (float)screen_w / (float)screen_h;

    float scale_x, scale_y, offset_x, offset_y;
    offset_x = 0.f;
    offset_y = 0.f;

    float w_screen_w, w_screen_h;

    if (s_aspect > w_aspect) {
        w_screen_h = screen_h;
        w_screen_w = screen_h * w_aspect;
        offset_x = (screen_w - w_screen_w) * 0.5f;
    } else {
        w_screen_w = screen_w;
        w_screen_h = screen_w / w_aspect;
        offset_y = (screen_h - w_screen_h) * 0.5f;
    }
    scale_x = w_screen_w / (float)WORLD_W;
    scale_y = w_screen_h / (float)WORLD_H;

    ClearBackground(GRAY);

    DrawRectangle((int)offset_x, (int)offset_y, (int)w_screen_w, (int)w_screen_h, BLACK);

    size_t players = input_manager_num_players();
    for (unsigned int i = 0; i < players; i++) {
        Player *p = &gs->players[i];
        float rx, ry, rw, rh;
        rx = p->x * scale_x + offset_x;
        ry = (WORLD_H - p->y - PLAYER_H) * scale_y + offset_y;
        rw = (float)PLAYER_W * scale_x;
        rh = (float)PLAYER_H * scale_y;

        int anim_sheet = player_idle_sprite;
        switch (p->state) {
            case PSIdle: anim_sheet = player_idle_sprite; break;
            case PSWalk: anim_sheet = player_walk_sprite; break;
            case PSPunch: anim_sheet = player_punch_sprite; break;
        }

        ssm_render(anim_sheet, rx, ry, rw, rh, p->anim_frame, p->facing != 1);
    }
}
