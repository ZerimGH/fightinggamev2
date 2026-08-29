#ifndef SERVER_H
#define SERVER_H

#include "server_info.h"
#include <stdint.h>

/* Initialise the server */
int server_init(char *name, uint8_t max_players);
/* Deinitialise the server */
void server_deinit(void);
/* Check if the server is initialised */
int server_is_init(void);
/* Update the server */
void server_update(void);
/* Check if the server has stopped */
int server_stopped(void);
/* Get the server's information */
ServerInfo server_get_info(void);
/* Count the number of connected clients */
uint8_t server_count_clients(void);

#endif /* SERVER_H */
