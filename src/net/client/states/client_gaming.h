#ifndef CLIENT_GAMING_H
#define CLIENT_GAMING_H

int client_gaming_enter(void);
void client_gaming_exit(void);
void client_gaming_update(void);
int client_gaming_get_input(uint8_t player_id, uint64_t frame, Input *input);
int client_gaming_send_input(TimedInput input);
int client_gaming_is_connected(uint8_t player_id, uint64_t frame);
uint8_t client_gaming_get_num_players(void);

#endif /* CLIENT_GAMING_H */
