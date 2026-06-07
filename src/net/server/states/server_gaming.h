#ifndef SERVER_GAMING_H
#define SERVER_GAMING_H

#include <stdint.h>

int server_gaming_enter(void);
void server_gaming_exit(void);
void server_gaming_update(void);
uint8_t server_gaming_count_clients(void);

#endif /* SERVER_GAMING_H */
