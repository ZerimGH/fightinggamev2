#ifndef CLIENT_PRIVATE_H
#define CLIENT_PRIVATE_H

#include "enet/enet.h"

typedef enum { CS_DISCOVERING, CS_WAITING, CS_GAMING } ClientState;

typedef struct {
    ENetHost *host;
    ENetPeer *peer;
    /* TODO: Gaming stuff */
} Client;

#ifndef CLIENT_MAIN
extern Client client;
#endif

int client_change_state(ClientState new);

#endif /* CLIENT_PRIVATE_H */
