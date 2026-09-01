#ifndef PLAYER_WALK_H
#define PLAYER_WALK_H

#include "input.h"
#include "player.h"

void player_walk_enter(Player *p);
void player_walk_exit(Player *p);
void player_walk_update(Player *p);
void player_walk_apply_inputs(Player *p, Input inputs, Input linputs);
void player_walk_do_collisions(Player *p1, Player *p2);

#endif /* PLAYER_WALK_H */
