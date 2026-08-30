#include "aabb.h"
#include "player.h"
#include <stdint.h>

int AABB_check_point(AABB *a, int16_t x, int16_t y) {
    if (!a) return 0;
    return x >= a->x && y >= a->y && x <= a->x + a->w && y <= a->y + a->h;
}

int AABB_check_AABB(AABB *a, AABB *b) {
    return AABB_check_point(a, b->x, b->y)
           || AABB_check_point(a, b->x + b->w, b->y)
           || AABB_check_point(a, b->x, b->y + b->h)
           || AABB_check_point(a, b->x + b->w, b->y + b->h);
}

AABB AABB_relative(AABB *a, Player *p) {
    AABB res = {0};
    if (!a || !p) return res;

    if (p->facing == 1) res.x = p->x + a->x;
    else res.x = p->x + PLAYER_RENDER_WIDTH - a->x - a->w;
    res.y = p->y + a->y;
    res.w = a->w;
    res.h = a->h;
    return res;
}
