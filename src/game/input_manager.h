#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

#include <stdint.h>
#include "input.h"

typedef enum { IM_LOCAL, IM_LAN } InputManagerType;

/* Initialise the input manager */
int input_manager_init(InputManagerType type);
/* Deinitialise the input manager */
void input_manager_deinit(void);
/* Return if the input manager has been initialised */
int input_manager_is_init(void);
/* Return the number of players whos inputs will be stored */
uint8_t input_manager_num_players(void);
/* Get new inputs for a specific player, returns 1 if no new inputs */
int input_manager_get_input(uint8_t player_id, Input *input);
/* Check if a player has disconnected */
int input_manager_disconnected(uint8_t player_id);
/* Update the input manager, should be called once per frame at 60fps */
void input_manager_tick(void);
/* Check if the input manager is finished */
int input_manager_over(void);

#endif /* INPUT_MANAGER_H */
