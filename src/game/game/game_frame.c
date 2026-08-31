#include "game_frame.h"
#include "global.h"
#include "input.h"
#include "input_manager.h"
#include "player.h"
#include <stddef.h>
#include <stdint.h>

void game_frame_init(GameFrame *gf) {
    if (!gf) { return; }

    gf->frame_no    = 0;
    gf->num_players = input_manager_num_players();

    for (unsigned int i = 0; i < gf->num_players; i++) {
        player_init(&gf->players[i], i, gf->num_players);
        gf->inputs[i].raw      = 0;
        gf->last_inputs[i].raw = 0;
    }
}

void game_frame_apply_input(GameFrame *gf, uint8_t player_id, Input input) {
    if (!gf) { return; }
    if (player_id >= gf->num_players) { return; }

    gf->last_inputs[player_id] = gf->inputs[player_id];
    gf->inputs[player_id]      = input;

    player_apply_inputs(&gf->players[player_id],
        gf->inputs[player_id],
        gf->last_inputs[player_id]);
}

void game_frame_finalise(GameFrame *gf) {
    if (!gf) { return; }

    for (uint8_t i = 0; i < gf->num_players; i++) {
        Player *p = &gf->players[i];
        player_update(p);
    }

    gf->frame_no++;
}
