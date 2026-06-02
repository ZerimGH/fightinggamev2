#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdint.h>
#include "input.h"

/* Initialise the input manager */
int input_manager_init(void); /* Will take client as an argument when I do network code */
/* Deinitialise the input manager */
void input_manager_deinit(void);
/* Return if the input manager has been initialised */
int input_manager_ready(void);
/* Return the number of players whos inputs will be stored */
uint8_t input_manager_num_players(void);
/* Get new inputs for a specific player, returns 1 if no new inputs */
int input_manager_get_input(uint8_t player_id, Input *input);
/* Update the input manager, should be called once per frame at 60fps */
void input_manager_tick(void);

/* TODO: Information about if a player is disconnected / missing inputs */

#endif /* INPUT_MANAGER_H */
