#ifndef CLIENT_PRIVATE_H
#define CLIENT_PRIVATE_H

#include "enet/enet.h"
#include "global.h"
#include "input.h"
#include <stdint.h>

typedef enum { CS_DISCOVERING, CS_WAITING, CS_GAMING } ClientState;

typedef struct {
    /* Circular buffer of inputs */
    TimedInput inputs[MAX_ROLLBACK];
    uint64_t head;
    /* Connected flag */
    int connected;
} PlayerInfo;

typedef struct {
    ENetHost *host;
    ENetPeer *peer;

    uint8_t num_players;
    PlayerInfo players[MAX_PLAYERS];

    uint8_t player_id;
} Client;

#ifndef CLIENT_MAIN
extern Client client;
#endif

int client_change_state(ClientState new);

#endif /* CLIENT_PRIVATE_H */
