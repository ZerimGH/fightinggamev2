#include "server_waiting.h"
#include "discovery.h"
#include "enet/enet.h"
#include "log.h"
#include "packet.h"
#include "server.h"
#include "server_info.h"
#include "server_private.h"

/* Discovery data */
static ENetSocket sock = ENET_SOCKET_NULL;
static ENetAddress addr = {0};

int server_waiting_enter(void) {
    /* Set up discovery stuff */
    /* Create a socket for discovery */
    sock = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    if (sock == ENET_SOCKET_NULL) return 1;
    if (enet_socket_set_option(sock, ENET_SOCKOPT_REUSEADDR, 1)) {
        enet_socket_destroy(sock);
        sock = ENET_SOCKET_NULL;
        return 1;
    }

    /* Bind socket to discovery address */
    addr.host = ENET_HOST_ANY;
    addr.port = DISCOVERY_PORT;

    if (enet_socket_bind(sock, &addr)) {
        enet_socket_destroy(sock);
        sock = ENET_SOCKET_NULL;
        return 1;
    }

    return 0;
}

void server_waiting_exit(void) {
    if (sock != ENET_SOCKET_NULL) enet_socket_destroy(sock);
    sock = ENET_SOCKET_NULL;
}

static void discover(void) {
    /* Not sure what this bit is for */
    ENetSocketSet set;
    ENET_SOCKETSET_EMPTY(set);
    ENET_SOCKETSET_ADD(set, sock);
    if (enet_socketset_select(sock, &set, NULL, 0) <= 0) return;

    /* Listen for clients */
    ENetAddress recvaddr;
    char buf;
    ENetBuffer recvbuf;
    recvbuf.data = &buf;
    recvbuf.dataLength = sizeof(buf);
    int recvlen = enet_socket_receive(sock, &recvaddr, &recvbuf, 1);
    if (recvlen <= 0) return;

    /* Reply with server info */
    ServerInfo info = server_get_info();
    recvbuf.data = &info;
    recvbuf.dataLength = sizeof(info);
    if (enet_socket_send(sock, &recvaddr, &recvbuf, 1) != (int)sizeof(info)) return;
}

static void handle_connect(ENetEvent event) {
    if (server.num_clients >= server.max_clients || server.num_clients >= MAX_PLAYERS) {
        enet_peer_disconnect_now(event.peer, 0);
        return;
    }
    ClientInfo *client = &server.clients[server.num_clients++];
    client->connected = 1;
    client->peer = event.peer;
    client->peer->data = (void *)client;
}

static void handle_disconnect(ENetEvent event) {
    /* Find disconnected client */
    for (int i = 0; i < server.num_clients; i++) {
        ClientInfo *client = &server.clients[i];
        if (!client->connected) continue;
        if (client->peer == event.peer) {
            /* Move all other clients to ensure player ids stay
             * in order */
            client->connected = 0;
            client->peer = NULL;
            /* This sucks, oh well */
            for (int j = i + 1; j < server.num_clients; j++) {
                server.clients[j - 1].connected = server.clients[j].connected;
                server.clients[j - 1].peer = server.clients[j].peer;
                server.clients[j - 1].peer->data = (void *)&server.clients[j - 1];
            }
            server.num_clients--;
            return;
        }
    }
}

void server_waiting_update(void) {
    /* Handle discovery */
    discover();
    /* Handle connections and disconnections */
    ENetEvent event;
    while (enet_host_service(server.host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: handle_connect(event); break;
            case ENET_EVENT_TYPE_DISCONNECT: handle_disconnect(event); break;
            case ENET_EVENT_TYPE_RECEIVE:
                /* Ignore packet and destroy */
                enet_packet_destroy(event.packet);
                break;
            default: break;
        }
    }

    /* Start the game if max_clients reached */
    if (server.num_clients >= server.max_clients || server.num_clients >= MAX_PLAYERS) {
        server_change_state(SS_GAMING);
    }
}
