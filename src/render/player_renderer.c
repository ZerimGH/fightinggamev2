#include "player_renderer.h"
#include "assets.h"
#include "letterbox.h"
#include "player.h"
#include "raylib/raylib.h"
#include "sprite_sheet_manager.h"
#include "world.h"

#define DO_MAGIC                                                                 \
    X(PLAYER_STATE_IDLE, player_idle, 1, 4)                                      \
    X(PLAYER_STATE_WALK, player_walk, 1, 3)                                      \
    X(PLAYER_STATE_PUNCH, player_punch, 1, 3) \
    X(PLAYER_STATE_HURT, player_hurt, 1, 4)

#define X(UNUSED, PREFIX, UNUSED2, UNUSED3) static int PREFIX##_sprite = -1;
DO_MAGIC
#undef X

int player_renderer_init(void) {
#define X(UNUSED, PREFIX, ROWS, COLS)                                            \
    PREFIX##_sprite = ssm_load(                                                  \
        ".png", PREFIX##_png, sizeof(PREFIX##_png), ROWS, COLS);                 \
    if (PREFIX##_sprite == -1) return 1;
    DO_MAGIC
#undef X
    return 0;
}

static int get_sprite(Player *p) {
#define X(STATE, PREFIX, UNUSED2, UNUSED3)                                       \
    case (STATE): return PREFIX##_sprite;
    switch (p->state) {
        DO_MAGIC
    default: return -1;
    }
}

static void player_draw_hitboxes(Player *p) {
    float sx, sy, sw, sh;
    AABB aabb = AABB_relative(&p->collision_box, p);
    letterbox_rect(aabb.x, aabb.y, aabb.w, aabb.h, &sx, &sy, &sw, &sh);
    DrawRectangle((int)sx,
        (int)sy,
        (int)sw,
        (int)sh,
        (Color){255, 0, 0, 32 + 64 * aabb.active});
    aabb = AABB_relative(&p->punch_box, p);
    letterbox_rect(aabb.x, aabb.y, aabb.w, aabb.h, &sx, &sy, &sw, &sh);
    DrawRectangle((int)sx,
        (int)sy,
        (int)sw,
        (int)sh,
        (Color){0, 0, 255, 32 + 64 * aabb.active});
}

void player_render(Player *p) {
    if (!p) return;
    float sx, sy, sw, sh;
    letterbox_rect(p->x,
        p->y,
        PLAYER_RENDER_WIDTH,
        PLAYER_RENDER_HEIGHT,
        &sx,
        &sy,
        &sw,
        &sh);
    int sprite = get_sprite(p);
    Color col  = p->connected ? WHITE : (Color){150, 150, 150, 128};
    ssm_render(sprite, sx, sy, sw, sh, p->anim_frame, p->facing != 1, col);
    // player_draw_hitboxes(p);
}
