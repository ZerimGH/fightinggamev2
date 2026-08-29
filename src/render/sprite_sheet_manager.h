#ifndef SPRITE_SHEET_MANAGER_H

#define SPRITE_SHEET_MANAGER_H

#define MAX_SPRITE_SHEETS 1024

#include "sprite_sheet.h"

int ssm_init(void);
int ssm_load(const char *ext, unsigned char *data, unsigned int len,
    unsigned int rows, unsigned int cols);
void ssm_render(int idx, float x, float y, float w, float h, unsigned int frame,
    int flip, Color col);
void ssm_deinit(void);

#endif /* SPRITE_SHEET_MANAGER_H */
