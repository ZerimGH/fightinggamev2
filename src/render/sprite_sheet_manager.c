#include "sprite_sheet_manager.h"
#include "log.h"
#include <stddef.h>

typedef struct {
    SpriteSheet sheets[MAX_SPRITE_SHEETS];
    unsigned int count;
} SpriteSheetManager;

static SpriteSheetManager ssm = {0};
static int init               = 0;

int ssm_init(void) {
    ssm.count = 0;
    init      = 1;
    return 0;
}

int ssm_load(const char *ext, unsigned char *data, unsigned int len,
    unsigned int rows, unsigned int cols) {
    if (!init) { return -1; }
    if (ssm.count >= MAX_SPRITE_SHEETS) { return -1; }
    if (sprite_sheet_init(&ssm.sheets[ssm.count], ext, data, len, rows, cols)) {
        PERROR("Failed to initialize sprite sheet.\n");
        return -1;
    }
    return ssm.count++;
}

static SpriteSheet *ssm_lookup(int idx) {
    if ((unsigned int)idx >= ssm.count || idx < 0) { return NULL; }
    return &ssm.sheets[idx];
}

void ssm_render(int idx, float x, float y, float w, float h, unsigned int frame,
    int flip, Color col) {
    SpriteSheet *ss = ssm_lookup(idx);
    if (!ss) { return; }
    sprite_sheet_render(ss, x, y, w, h, frame, flip, col);
}

void ssm_deinit(void) {
    for (unsigned int i = 0; i < ssm.count; i++) {
        sprite_sheet_deinit(&ssm.sheets[i]);
    }
    ssm.count = 0;
    init      = 0;
}
