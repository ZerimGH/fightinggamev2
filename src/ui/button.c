#include "button.h"
#include "raylib/raylib.h"

static Rectangle button_to_rect(int idx, int tot) {
    float width = 0.4f;
    float height = 0.1f;
    float spacing = 0.02f;

    float screen_w = (float)GetScreenWidth();
    float screen_h = (float)GetScreenHeight();

    float pix_w = screen_w * width;
    float pix_h = screen_h * height;
    float space_pix = screen_h * spacing;

    float total_height = (tot * pix_h) + ((tot - 1) * space_pix);

    float x = (screen_w - pix_w) / 2.0f;
    float start_y = (screen_h - total_height) / 2.0f;
    float y = start_y + (idx * (pix_h + space_pix));

    return (Rectangle){x, y, pix_w, pix_h};
}

void button_render(Button *button, int idx, int tot) {
    if (!button) {
        return;
    }

    Rectangle rect = button_to_rect(idx, tot);
    int font_size = (int)(rect.height * 0.4f);

    Color col = GRAY;
    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        col = LIGHTGRAY;
    }
    if (!button->active) {
        col = DARKGRAY;
    }

    DrawRectangleRec(rect, col);
    DrawRectangleLinesEx(rect, 2, DARKGRAY);

    int text_width = MeasureText(button->text, font_size);
    DrawText(button->text, (int)(rect.x + (rect.width - text_width) / 2), (int)(rect.y + (rect.height - font_size) / 2),
             font_size, WHITE);
}

void button_set_active(Button *button, int val) {
    if (!button) {
        return;
    }
    button->active = val;
}

int button_pressed(Button *button, int idx, int tot) {
    if (!button || !button->active) {
        return 0;
    }

    Rectangle rect = button_to_rect(idx, tot);

    int click = IsMouseButtonReleased(MOUSE_BUTTON_LEFT);

    if (CheckCollisionPointRec(GetMousePosition(), rect)) {
        if (click) {
            return 1;
        }
    }

    return 0;
}
