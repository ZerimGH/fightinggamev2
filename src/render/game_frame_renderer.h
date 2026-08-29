#ifndef GAME_FRAME_RENDERER_H
#define GAME_FRAME_RENDERER_H

#include "game_frame.h"

int game_frame_renderer_init(void);
void game_frame_render(GameFrame *gf);
void game_frame_renderer_deinit(void);

#endif /* GAME_FRAME_RENDERER_H */
