#ifndef SERVER_PRIVATE_H
#define SERVER_PRIVATE_H

#include <stdint.h>
#include "enet/enet.h"
#include "global.h"
#include "server_info.h"

typedef enum {
    SS_WAITING, /* Waiting for clients */
    SS_GAMING   /* Game has started */
} ServerState;

typedef struct {
    int connected;
    uint8_t player_id;
    ENetPeer *peer;
} ClientInfo;

typedef struct {
    ENetHost *host;
    uint8_t max_clients;
    int num_clients;
    ClientInfo clients[MAX_PLAYERS];
    char name[SERVER_NAME_SIZE];
} Server;

#ifndef SERVER_MAIN
extern Server server; /* Defined in src/net/server/server.c */
#endif

int server_change_state(ServerState new);

#endif /* SERVER_PRIVATE_H */
