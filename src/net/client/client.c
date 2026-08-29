#include "client.h"
#include "client_discovering.h"
#include "client_gaming.h"
#include "client_private.h"
#include "client_waiting.h"
#include "enet/enet.h"
#include "log.h"

#define DO_MAGIC                                                                 \
    X(CLIENT_STATE_DISCOVERING, client_discovering)                                        \
    X(CLIENT_STATE_WAITING, client_waiting)                                                \
    X(CLIENT_STATE_GAMING, client_gaming)

Client client            = {0};
static ClientState state = (ClientState)-1;
static int init          = 0;

static int client_state_enter(ClientState new);

int client_init(void) {
    if (init) {
        PERROR("init() called when already initialised\n");
        return 1;
    }

    /* Create client host */
    client.host = enet_host_create(NULL, 1, 1, 0, 0);
    if (!client.host) {
        PERROR("Failed to create client host\n");
        return 1;
    }

    /* Set state */
    if (client_state_enter(CLIENT_STATE_DISCOVERING)) {
        PERROR("Failed to enter discovering state\n");
        enet_host_destroy(client.host);
        client.host = NULL;
        return 1;
    }

    init = 1;
    return 0;
}

void client_deinit(void) {
    if (!init) {
        PERROR("(warn) deinit() called when not initialised\n");
        return;
    }

    if (client.peer) enet_peer_disconnect_now(client.peer, 0);

    if (client.host) enet_host_destroy(client.host);
    client.host = NULL;
    init        = 0;
}

int client_is_init(void) { return init; }

int client_get_servers(ServerInfo *buf, int buf_len) {
    if (!init || state != CLIENT_STATE_DISCOVERING) return 0;
    return client_discovering_get_servers(buf, buf_len);
}

int client_state_change(ClientState new);

int client_join(uint32_t host, uint16_t port) {
    if (!init) {
        PERROR("(warn) join() called when not initialised\n");
        return 1;
    }

    /* Create the peer */
    ENetAddress addr;
    if (host == ENET_HOST_ANY) {
        addr.host = 0x0100007F; /* 127.0.0.1 */
    } else {
        addr.host = host;
    }
    addr.port   = port;
    client.peer = enet_host_connect(client.host, &addr, 1, 0);
    if (!client.peer) return 1;

    if (client_state_change(CLIENT_STATE_WAITING)) {
        PERROR("Failed to enter waiting state\n");
        enet_peer_disconnect_now(client.peer, 0);
        return 1;
    }

    return 0;
}

int client_disconnected(void) { return !client.peer; }

void client_disconnect(void) {
    if (client.peer) enet_peer_disconnect_now(client.peer, 0);
    client.peer = NULL;
}

int client_started(void) { return state == CLIENT_STATE_GAMING; }

int client_get_input(uint8_t player_id, uint64_t frame, Input *input) {
    if (state != CLIENT_STATE_GAMING) return 1;
    return client_gaming_get_input(player_id, frame, input);
}

int client_send_input(TimedInput input) {
    if (state != CLIENT_STATE_GAMING) return 1;
    return client_gaming_send_input(input);
}

int client_is_connected(uint8_t player_id, uint64_t frame) {
    if (state != CLIENT_STATE_GAMING) return 0;
    return client_gaming_is_connected(player_id, frame);
}

uint8_t client_get_num_players(void) {
    if (state != CLIENT_STATE_GAMING) return 0;
    return client_gaming_get_num_players();
}

int client_connection_confirmed(void) {
    if (state != CLIENT_STATE_WAITING) return 0;
    return client_waiting_connection_confirmed();
}

/* State machine stuff */
void client_update(void) {
    if (!init) {
        PERROR("(warn) update() called when not initialised\n");
        return;
    }

#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_update(); break;

    switch (state) { DO_MAGIC }
#undef X
    enet_host_flush(client.host);
}

static void client_state_exit(void) {
#define X(STATE, PREFIX)                                                         \
    case (STATE): PREFIX##_exit(); return;
    switch (state) { DO_MAGIC }
#undef X
}

static int client_state_enter(ClientState new) {
    int stat = 1;
#define X(STATE, PREFIX)                                                         \
    case (STATE): stat = PREFIX##_enter(); break;

    switch (new) { DO_MAGIC }
#undef X

    if (stat) {
        state = (ClientState)-1; /* Not sure how to handle this */
        return 1;
    }
    state = new;
    return 0;
}

int client_state_change(ClientState new) {
    client_state_exit();
    return client_state_enter(new);
}
