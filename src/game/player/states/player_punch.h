#ifndef PLAYER_PUNCH_H
#define PLAYER_PUNCH_H

#include "input.h"
#include "player.h"

void player_punch_enter(Player *p);
void player_punch_exit(Player *p);
void player_punch_update(Player *p);
void player_punch_apply_inputs(Player *p, Input inputs, Input linputs);

#endif /* PLAYER_PUNCH_H */
