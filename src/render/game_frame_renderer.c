#include "assets.h"
#include "game_frame.h"
#include "log.h"
#include "raylib/raylib.h"
#include "sprite_sheet_manager.h"
#include "sprite_sheets.h"

int game_frame_renderer_init(void) {
    if (ssm_init()) return 1;
    player_idle_sprite  = ssm_load(".png", idle_png, sizeof(idle_png), 1, 4);
    player_walk_sprite  = ssm_load(".png", walk_png, sizeof(walk_png), 1, 3);
    player_punch_sprite = ssm_load(".png", punch_png, sizeof(punch_png), 1, 3);
    if (player_walk_sprite == -1 || player_idle_sprite == -1
        || player_punch_sprite == -1) {
        ssm_deinit();
        return 1;
    }

    return 0;
}

void game_frame_render(GameFrame *gf) {
    if (!gf) { return; }

    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();

    float w_aspect = (float)WORLD_W / (float)WORLD_H;
    float s_aspect = (float)screen_w / (float)screen_h;

    float scale_x, scale_y, offset_x, offset_y;
    offset_x = 0.f;
    offset_y = 0.f;

    float w_screen_w, w_screen_h;

    if (s_aspect > w_aspect) {
        w_screen_h = screen_h;
        w_screen_w = screen_h * w_aspect;
        offset_x   = (screen_w - w_screen_w) * 0.5f;
    } else {
        w_screen_w = screen_w;
        w_screen_h = screen_w / w_aspect;
        offset_y   = (screen_h - w_screen_h) * 0.5f;
    }
    scale_x = w_screen_w / (float)WORLD_W;
    scale_y = w_screen_h / (float)WORLD_H;

    ClearBackground(DARKGRAY);

    DrawRectangle(
        (int)offset_x, (int)offset_y, (int)w_screen_w, (int)w_screen_h, BLACK);

    for (unsigned int i = 0; i < gf->num_players; i++) {
        Player *p = &gf->players[i];
        float rx, ry, rw, rh;
        rx = p->x * scale_x + offset_x;
        ry = (WORLD_H - p->y - PLAYER_H) * scale_y + offset_y;
        rw = (float)PLAYER_W * scale_x;
        rh = (float)PLAYER_H * scale_y;

        int anim_sheet = player_idle_sprite;
        switch (p->state) {
        case PSIdle:  anim_sheet = player_idle_sprite; break;
        case PSWalk:  anim_sheet = player_walk_sprite; break;
        case PSPunch: anim_sheet = player_punch_sprite; break;
        }

        Color col = p->connected ? WHITE : (Color){150, 150, 150, 128};
        ssm_render(
            anim_sheet, rx, ry, rw, rh, p->anim_frame, p->facing != 1, col);
    }
}

void game_frame_renderer_deinit(void) { ssm_deinit(); }
