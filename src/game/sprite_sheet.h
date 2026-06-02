#ifndef SPRITE_SHEET_H

#define SPRITE_SHEET_H

#include "raylib/raylib.h"

typedef struct {
    Texture2D texture;
    unsigned int rows, cols;
    unsigned int sprite_width, sprite_height;
} SpriteSheet;

int sprite_sheet_init(SpriteSheet *ss, const char *ext, unsigned char *data, unsigned int len, unsigned int rows,
                      unsigned int cols);
void sprite_sheet_render(SpriteSheet *ss, float x, float y, float w, float h, unsigned int frame, int flip);
void sprite_sheet_deinit(SpriteSheet *ss);

#endif /* SPRITE_SHEET_H */
