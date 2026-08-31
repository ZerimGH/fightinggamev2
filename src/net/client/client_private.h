#ifndef CLIENT_PRIVATE_H
#define CLIENT_PRIVATE_H

#include "enet/enet.h"
#include "global.h"
#include "input.h"
#include <stdint.h>

typedef enum {
    CLIENT_STATE_DISCOVERING,
    CLIENT_STATE_WAITING,
    CLIENT_STATE_GAMING
} ClientState;

typedef struct {
    /* Circular buffer of inputs */
    TimedInput inputs[MAX_ROLLBACK];
    uint64_t head;
    /* Connected flag */
    int connected;
    uint64_t disconnect_frame;
} PlayerInfo;

typedef struct {
    ENetHost *host;
    ENetPeer *peer;

    uint8_t num_players;
    PlayerInfo players[MAX_PLAYERS];

    uint32_t delay;
    uint8_t player_id;
} Client;

extern Client client;

int client_state_change(ClientState new);

#endif /* CLIENT_PRIVATE_H */
