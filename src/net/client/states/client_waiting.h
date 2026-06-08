#ifndef CLIENT_WAITING_H
#define CLIENT_WAITING_H

int client_waiting_enter(void);
void client_waiting_exit(void);
void client_waiting_update(void);
int client_waiting_connection_confirmed(void);

#endif /* CLIENT_WAITING_H */
