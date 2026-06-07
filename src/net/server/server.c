#include <stdio.h>
#include <string.h>
#include "enet/enet.h"
#include "global.h"
#include "log.h"
#include "server.h"
#include "server_info.h"
#define SERVER_MAIN
#include "server_private.h"
#include "server_waiting.h"
#include "server_gaming.h"

Server server = {0};
static ServerState state = (ServerState)-1;
static int init = 0;

static int server_state_enter(ServerState new);

int server_init(char *name, uint8_t max_players) {
    if (init) {
        PERROR("init() called when already initialised\n");
        return 1;
    }

    if (max_players < 2) {
        PERROR("Cannot start a server with less than two players\n");
        return 1;
    }

    if (!name || strlen(name) < 1) {
        PERROR("Server name too short\n");
        return 1;
    }

    /* Create server host */
    ENetAddress addr;
    addr.host = ENET_HOST_ANY;
    addr.port = ENET_PORT_ANY;
    server.host = enet_host_create(&addr, MAX_PLAYERS, 0, 0, 0);
    if (!server.host) {
        PERROR("Failed to create server host\n");
        return 1;
    }

    /* Get actual port of server */
    ENetAddress real_addr;
    enet_socket_get_address(server.host->socket, &real_addr);
    server.host->address.port = real_addr.port;

    /* Initialise client list */
    server.num_clients = 0;
    for (int i = 0; i < MAX_PLAYERS; i++) {
        server.clients[i] = (ClientInfo){.player_id = (uint8_t)i, .peer = NULL, .latest_input = (uint64_t)-1};
    }

    /* Copy server options */
    snprintf(server.name, SERVER_NAME_SIZE, "%s", name);
    server.max_clients = max_players;

    /* Set state */
    if (server_state_enter(SS_WAITING)) {
        PERROR("Failed to enter waiting state\n");
        enet_host_destroy(server.host);
        server.host = NULL;
        return 1;
    }

    init = 1;
    return 0;
}

void server_deinit(void) {
    if (!init) {
        PERROR("(warn) deinit() called when not initialised\n");
        return;
    }

    if (server.host) {
        if (server.num_clients) {
            for (int i = 0; i < server.num_clients; i++) {
                ClientInfo *client = &server.clients[i];
                if (!client->peer) continue;
                enet_peer_disconnect_now(client->peer, 0);
            }
        }
        enet_host_destroy(server.host);
        server.host = NULL;
    }
    state = (ServerState)-1;
    init = 0;
}

int server_is_init(void) {
    return init;
}

void server_update(void) {
    if (!init) {
        PERROR("(warn) update() called when not initialised\n");
        return;
    }

    switch (state) {
        case SS_WAITING: server_waiting_update(); break;
        case SS_GAMING: server_gaming_update(); return;
        default: return;
    }

    enet_host_flush(server.host);
}

ServerInfo server_get_info(void) {
    if (!init) {
        PERROR("(warn) get_info() called when not initialised\n");
        return (ServerInfo){0};
    }

    if (!server.host) {
        PERROR("(warn) get_info() called, but server.host was NULL\n");
        return (ServerInfo){0};
    }

    ServerInfo info = {0};
    info.host = server.host->address.host;
    info.port = server.host->address.port;

    snprintf(info.name, sizeof(info.name), "%s", server.name);

    info.max_players = server.max_clients;
    info.cur_players = server.num_clients;

    return info;
}

/* State machine stuff */
static void server_state_exit(void) {
    switch (state) {
        case SS_WAITING: server_waiting_exit(); return;
        case SS_GAMING: server_gaming_exit(); return;
    }
}

static int server_state_enter(ServerState new) {
    int stat = 1;
    switch (new) {
        case SS_WAITING: stat = server_waiting_enter(); break;
        case SS_GAMING: stat = server_gaming_enter(); break;
    }
    if (stat) {
        state = (ServerState)-1; /* Not sure how to handle this */
        return 1;
    }
    state = new;
    return 0;
}

int server_change_state(ServerState new) {
    server_state_exit();
    return server_state_enter(new);
}

uint8_t server_count_clients(void) {
    if (state != SS_GAMING) return server.num_clients;
    return server_gaming_count_clients();
}
