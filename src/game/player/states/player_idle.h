#ifndef PLAYER_IDLE_H
#define PLAYER_IDLE_H

#include "input.h"
#include "player.h"

void player_idle_enter(Player *p);
void player_idle_exit(Player *p);
void player_idle_update(Player *p);
void player_idle_apply_inputs(Player *p, Input inputs, Input linputs);

#endif /* PLAYER_IDLE_H */
