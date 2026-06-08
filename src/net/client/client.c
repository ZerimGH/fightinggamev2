#include "client.h"
#include "client_discovering.h"
#include "client_private.h"
#include "client_waiting.h"
#include "client_gaming.h"
#include "enet/enet.h"
#include "log.h"

Client client = {0};
static ClientState state = (ClientState)-1;
static int init = 0;

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
    if (client_state_enter(CS_DISCOVERING)) {
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
    init = 0;
}

int client_is_init(void) {
    return init;
}

int client_get_servers(ServerInfo *buf, int buf_len) {
    if (!init || state != CS_DISCOVERING) return 0;
    return client_discovering_get_servers(buf, buf_len);
}

int client_change_state(ClientState new);

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
    addr.port = port;
    client.peer = enet_host_connect(client.host, &addr, 1, 0);
    if (!client.peer) return 1;

    if (client_change_state(CS_WAITING)) {
        PERROR("Failed to enter waiting state\n");
        enet_peer_disconnect_now(client.peer, 0);
        return 1;
    }

    return 0;
}

void client_update(void) {
    if (!init) {
        PERROR("(warn) update() called when not initialised\n");
        return;
    }

    switch (state) {
        case CS_DISCOVERING: client_discovering_update(); break;
        case CS_WAITING: client_waiting_update(); break;
        case CS_GAMING: client_gaming_update(); break;
    }
    enet_host_flush(client.host);
}

int client_disconnected(void) {
    return !client.peer;
}

void client_disconnect(void) {
    if (client.peer) enet_peer_disconnect_now(client.peer, 0);
    client.peer = NULL;
}

int client_started(void) {
    return state == CS_GAMING;
}

/* State machine stuff */
static void client_state_exit(void) {
    switch (state) {
        case CS_DISCOVERING: client_discovering_exit(); return;
        case CS_WAITING: client_waiting_exit(); return;
        case CS_GAMING: client_gaming_exit(); return;
    }
}

static int client_state_enter(ClientState new) {
    int stat = 1;
    switch (new) {
        case CS_DISCOVERING: stat = client_discovering_enter(); break;
        case CS_WAITING: stat = client_waiting_enter(); break;
        case CS_GAMING: stat = client_gaming_enter(); break;
    }
    if (stat) {
        state = (ClientState)-1; /* Not sure how to handle this */
        return 1;
    }
    state = new;
    return 0;
}

int client_change_state(ClientState new) {
    client_state_exit();
    return client_state_enter(new);
}

int client_get_input(uint8_t player_id, uint64_t frame, Input *input) {
    if (state != CS_GAMING) return 1;
    return client_gaming_get_input(player_id, frame, input);
}

int client_send_input(TimedInput input) {
    if (state != CS_GAMING) return 1;
    return client_gaming_send_input(input);
}

int client_is_connected(uint8_t player_id, uint64_t frame) {
    if (state != CS_GAMING) return 0;
    return client_gaming_is_connected(player_id, frame);
}

uint8_t client_get_num_players(void) {
    if (state != CS_GAMING) return 0;
    return client_gaming_get_num_players();
}

int client_connection_confirmed(void) {
    if (state != CS_WAITING) return 0;
    return client_waiting_connection_confirmed();
}

