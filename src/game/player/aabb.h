#ifndef AABB_H
#define AABB_H

#include <stdint.h>

typedef struct Player Player;

typedef struct {
    int16_t x, y, w, h;
    int active;
} AABB;

int AABB_check_AABB(AABB *a, AABB *b);
int AABB_check_point(AABB *a, int16_t x, int16_t y);
AABB AABB_relative(AABB *a, Player *p);

#endif /* AABB_H */
