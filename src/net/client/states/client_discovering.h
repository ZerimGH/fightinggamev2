#ifndef CLIENT_DISCOVERING_H
#define CLIENT_DISCOVERING_H

#include "server_info.h"

int client_discovering_enter(void);
void client_discovering_exit(void);
void client_discovering_update(void);
int client_discovering_get_servers(ServerInfo *buf, int buf_len);

#endif /* CLIENT_DISCOVERING_H */
