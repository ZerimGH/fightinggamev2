#ifndef GAME_STATE_RENDERER_H
#define GAME_STATE_RENDERER_H

#include "game_state.h"

int game_state_renderer_init(void);
void game_state_render(GameState *gs);
void game_state_renderer_deinit(void);

#endif /* GAME_STATE_RENDERER_H */
