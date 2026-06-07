#include "button.h"
#include "raylib/raylib.h"

#define MAX_FONT_SIZE_MULT 0.04f
#define MIN_FONT_SIZE      14
#define SPACING_H          24.0f
#define SPACING_W          10.0f

static void calculate_layout(int tot, int *out_font_size, float *out_row_w, float *out_row_h, int *out_cols,
                             float *out_grid_h) {
    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();

    int max_font_size = (int)(screen_h * MAX_FONT_SIZE_MULT);
    if (max_font_size < MIN_FONT_SIZE) max_font_size = MIN_FONT_SIZE;

    int font_size = max_font_size;
    int cols = 1;

    float max_h = screen_h * 0.8f;
    float cur_h = 0.0f;

    while (font_size >= MIN_FONT_SIZE) {
        float row_h = (float)font_size + SPACING_H;
        int col_len = (tot + cols - 1) / cols;

        cur_h = (col_len * row_h) + ((col_len - 1) * SPACING_W);

        if (cur_h <= max_h) break;

        /* TODO: Dynamically add columns */
        if (cols == 1) cols = 2;
        else font_size -= 2;
    }

    if (font_size < MIN_FONT_SIZE) font_size = MIN_FONT_SIZE;

    float row_w = (screen_w - 200.0f);
    if (cols > 1) row_w = (screen_w - 240.0f) / 2.0f;

    *out_font_size = font_size;
    *out_row_h = (float)font_size + SPACING_H;
    *out_row_w = row_w;
    *out_cols = cols;
    *out_grid_h = cur_h;
}

static Rectangle button_to_dynamic_rect(int idx, int tot) {
    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();

    int font_size;
    float row_w, row_h, grid_h;
    int cols;

    calculate_layout(tot, &font_size, &row_w, &row_h, &cols, &grid_h);

    int col_len = (tot + cols - 1) / cols;
    int col = idx / col_len;
    int row = idx % col_len;

    float start_y = (screen_h - grid_h) / 2.0f;
    float start_x = 100.0f;

    if (cols == 1) start_x = (screen_w - row_w) / 2.0f;
    else {
        float grid_w = (cols * row_w) + ((cols - 1) * 40.0f);
        start_x = (screen_w - grid_w) / 2.0f;
    }

    float dx = col * (row_w + 40.0f);
    float dy = row * (row_h + SPACING_W);

    return (Rectangle){start_x + dx, start_y + dy, row_w, row_h};
}

void button_render(Button *button, int idx, int tot) {
    if (!button) return;

    Rectangle rect = button_to_dynamic_rect(idx, tot);

    int font_size;
    float row_w, row_h, grid_h;
    int cols;
    calculate_layout(tot, &font_size, &row_w, &row_h, &cols, &grid_h);

    Vector2 mouse_pos = GetMousePosition();
    int selected = CheckCollisionPointRec(mouse_pos, rect);

    Color panel_colour = selected ? DARKGRAY : GetColor(0x1F1F1F);
    Color border_colour = selected ? RED : GRAY;
    Color text_colour = WHITE;

    if (!button->active) {
        panel_colour = GetColor(0x121212);
        border_colour = DARKGRAY;
        text_colour = GRAY;
    }

    DrawRectangleRec(rect, panel_colour);
    DrawRectangleLinesEx(rect, 2.0f, border_colour);

    int text_w = MeasureText(button->text, font_size);
    int text_x = (int)(rect.x + (rect.width - text_w) / 2.0f);
    int text_y = (int)(rect.y + (rect.height - font_size) / 2.0f);

    DrawText(button->text, text_x, text_y, font_size, text_colour);
}

void button_set_active(Button *button, int val) {
    if (!button) return;
    button->active = val;
}

int button_pressed(Button *button, int idx, int tot) {
    if (!button || !button->active) return 0;

    Rectangle rect = button_to_dynamic_rect(idx, tot);
    Vector2 mouse_pos = GetMousePosition();

    if (CheckCollisionPointRec(mouse_pos, rect) && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) return 1;

    return 0;
}
