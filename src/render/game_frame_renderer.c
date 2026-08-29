#include "game_frame.h"
#include "letterbox.h"
#include "player_renderer.h"
#include "raylib/raylib.h"
#include "sprite_sheet_manager.h"
#include "world.h"
#include <stdio.h>

int game_frame_renderer_init(void) {
    if (ssm_init()) return 1;
    if (player_renderer_init()) {
        ssm_deinit();
        return 1;
    }

    return 0;
}

void game_frame_render(GameFrame *gf) {
    if (!gf) { return; }
    ClearBackground(DARKGRAY);
    float sx, sy, sw, sh;
    letterbox_rect(0, 0, WORLD_WIDTH, WORLD_HEIGHT, &sx, &sy, &sw, &sh);
    DrawRectangle((int)sx, (int)sy, (int)sw, (int)sh, BLACK);
    for (int i = 0; i < gf->num_players; i++) { player_render(&gf->players[i]); }
}

void game_frame_renderer_deinit(void) { ssm_deinit(); }
