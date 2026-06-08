#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include "server_info.h"
#include "input.h"

/* Initialise the client */
int client_init(void);
/* Deinitialise the client */
void client_deinit(void);
/* Check if the client is initialised */
int client_is_init(void);
/* Get a list of servers found by the client 
 * Returns number of servers written */
int client_get_servers(ServerInfo *buf, int buf_len);
/* Join a server */
int client_join(uint32_t host, uint16_t port);
/* Update the client */
void client_update(void);
/* Check if the client has been disconnected */
int client_disconnected(void);
/* Disconnect the client */
void client_disconnect(void);
/* Check if the client has received a start packet */
int client_started(void);
/* Check if the client has connected to a server and
 * received confirmation */
int client_connection_confirmed(void);

/* Get a player's input on a frame
 * Returns 0 and writes to input on success */
int client_get_input(uint8_t player_id, uint64_t frame, Input *input);
/* Send an input to the server */
int client_send_input(TimedInput input);
/* Check if a player is connected */
int client_is_connected(uint8_t player_id);
/* Get the player id assigned to the client */
uint8_t client_get_id(void);
/* Get the number of players client started with */
uint8_t client_get_num_players(void);


#endif /* CLIENT_H */
