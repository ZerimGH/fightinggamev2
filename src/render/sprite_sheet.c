#include "sprite_sheet.h"
#include "log.h"

int sprite_sheet_init(SpriteSheet *ss, const char *ext, unsigned char *data,
    unsigned int len, unsigned int rows, unsigned int cols) {
    if (!ss || !ext || !data) { return 1; }
    if (rows == 0 || cols == 0) {
        PERROR("Invalid number of rows or columns.\n");
        return 1;
    }

    Image img = LoadImageFromMemory(ext, data, len);
    if (img.width == 0 || img.height == 0) {
        PERROR("Failed to load image\n");
        return 1;
    }
    ss->texture = LoadTextureFromImage(img);
    UnloadImage(img);

    ss->rows          = rows;
    ss->cols          = cols;
    ss->sprite_width  = ss->texture.width / ss->cols;
    ss->sprite_height = ss->texture.height / ss->rows;
    return 0;
}

void sprite_sheet_render(SpriteSheet *ss, float x, float y, float w, float h,
    unsigned int frame, int flip, Color col) {
    if (!ss) { return; }
    unsigned int num_frames = ss->rows * ss->cols;
    frame %= num_frames;

    unsigned int sprite_x = (frame % ss->cols) * ss->sprite_width;
    unsigned int sprite_y = (frame / ss->cols) * ss->sprite_height;

    Rectangle src  = {(float)sprite_x,
        (float)sprite_y,
        (float)ss->sprite_width,
        (float)ss->sprite_height};
    Rectangle dest = {x, y, w, h};

    if (flip) { src.width *= -1; }

    DrawTexturePro(ss->texture, src, dest, (Vector2){0, 0}, 0.0f, col);
}

void sprite_sheet_deinit(SpriteSheet *ss) {
    if (!ss) { return; }
    UnloadTexture(ss->texture);
}
