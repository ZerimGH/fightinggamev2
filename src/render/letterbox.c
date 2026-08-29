#include "raylib/raylib.h"
#include "world.h"

static void calculate_letterbox(
    float *scale_x, float *scale_y, float *offset_x, float *offset_y) {
    int screen_w = GetScreenWidth();
    int screen_h = GetScreenHeight();

    float w_aspect = (float)WORLD_WIDTH / (float)WORLD_HEIGHT;
    float s_aspect = (float)screen_w / (float)screen_h;

    float w_screen_w, w_screen_h;
    *offset_x = 0.0f;
    *offset_y = 0.0f;

    if (s_aspect > w_aspect) {
        w_screen_h = (float)screen_h;
        w_screen_w = w_screen_h * w_aspect;
        *offset_x  = (screen_w - w_screen_w) * 0.5f;
    } else {
        w_screen_w = (float)screen_w;
        w_screen_h = w_screen_w / w_aspect;
        *offset_y  = (screen_h - w_screen_h) * 0.5f;
    }

    *scale_x = w_screen_w / (float)WORLD_WIDTH;
    *scale_y = w_screen_h / (float)WORLD_HEIGHT;
}

void letterbox_rect(float wx, float wy, float ww, float wh, float *sx, float *sy,
    float *sw, float *sh) {
    float scale_x, scale_y, offset_x, offset_y;
    calculate_letterbox(&scale_x, &scale_y, &offset_x, &offset_y);
    if (sw) *sw = scale_x * ww;
    if (sh) *sh = scale_y * wh;
    if (sx) *sx = scale_x * wx + offset_x;
    if (sy) *sy = scale_y * ((float)WORLD_HEIGHT - wy - wh) + offset_y;
}
