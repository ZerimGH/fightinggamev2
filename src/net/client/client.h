#ifndef CLIENT_H
#define CLIENT_H

#include <stdint.h>
#include "server_info.h"

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

/* TODO: Gaming state functions */

#endif /* CLIENT_H */
