#ifndef PLAYER_HURT_H
#define PLAYER_HURT_H

#include "input.h"
#include "player.h"

void player_hurt_enter(Player *p);
void player_hurt_exit(Player *p);
void player_hurt_update(Player *p);
void player_hurt_apply_inputs(Player *p, Input inputs, Input linputs);
void player_hurt_do_collisions(Player *p1, Player *p2);

#endif /* PLAYER_HURT_H */
