#ifndef SERVER_PRIVATE_H
#define SERVER_PRIVATE_H

#include "enet/enet.h"
#include "global.h"
#include "input.h"
#include "server_info.h"
#include <stdint.h>

typedef enum {
    SS_WAITING, /* Waiting for clients */
    SS_GAMING   /* Game has started */
} ServerState;

typedef struct {
    uint8_t player_id;
    ENetPeer *peer;

    TimedInput latest_input;
    uint64_t predict_to;
} ClientInfo;

typedef struct {
    ENetHost *host;
    uint8_t max_clients;
    int num_clients;
    ClientInfo clients[MAX_PLAYERS];
    char name[SERVER_NAME_SIZE];
    uint64_t max_frame;
} Server;

extern Server server; /* Defined in src/net/server/server.c */

int server_state_change(ServerState new);

#endif /* SERVER_PRIVATE_H */
