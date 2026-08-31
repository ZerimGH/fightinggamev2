#include "server_sync.h"
#include "enet/enet.h"
#include "global.h"
#include "log.h"
#include "packet.h"
#include "server.h"
#include "server_private.h"
#include <stdio.h>

static uint32_t send_times[MAX_PLAYERS] = {0};
static uint32_t recv_times[MAX_PLAYERS] = {0};
static int received                     = 0;

int server_sync_enter(void) {
    /* Send a sync packet to all clients */
    PacketServerSync server_sync = {0};
    Packet packet = packet_create(PACKET_SERVER_SYNC, &server_sync);
    for (int i = 0; i < server.num_clients; i++) {
        if (packet_send(packet, server.clients[i].peer)) {
            /* TODO: Handle error */
            continue;
        }
        send_times[i] = enet_time_get();
        recv_times[i] = 0;
    }
    received = 0;
    return 0;
}

void server_sync_exit(void) {}

static void handle_disconnect(ENetEvent event) {
    /* Find disconnected client in client list */
    for (int i = 0; i < server.num_clients; i++) {
        ClientInfo *client = &server.clients[i];
        if (!client->peer) continue;
        if (client->peer == event.peer) {
            /* Move all other clients to ensure player ids stay
             * in order */
            client->peer = NULL;
            /* This sucks, oh well */
            for (int j = i + 1; j < server.num_clients; j++) {
                server.clients[j - 1].peer       = server.clients[j].peer;
                server.clients[j - 1].peer->data = (void *)&server.clients[j - 1];
            }
            server.num_clients--;
            return;
        }
    }
}

static void handle_connect(ENetEvent event) {
    enet_peer_disconnect_now(event.peer, 0);
}

static void handle_receive(ENetEvent event) {
    Packet packet = packet_parse(event.packet);
    if (packet.type == PACKET_CLIENT_SYNC) {
        ClientInfo *client = event.peer->data;
        if (recv_times[client->player_id] == 0) {
            recv_times[client->player_id] = enet_time_get();
            received++;
        }
    }
    enet_packet_destroy(event.packet);
}

void server_sync_update(void) {
    ENetEvent event;
    while (enet_host_service(server.host, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_CONNECT:    handle_connect(event); break;
        case ENET_EVENT_TYPE_DISCONNECT: handle_disconnect(event); break;
        case ENET_EVENT_TYPE_RECEIVE:    handle_receive(event);
        default:                         break;
        }
    }

    if (received == server.num_clients) {
        uint32_t delay_max = 0;
        for (int i = 0; i < server.num_clients; i++) {
            uint32_t delay = (recv_times[i] - send_times[i]) / 2;
            if (delay > delay_max) delay_max = delay;
        }

        for (int i = 0; i < server.num_clients; i++) {
            uint32_t delay          = (recv_times[i] - send_times[i]) / 2;
            server.clients[i].delay = delay_max - delay;
        }
        server_state_change(SERVER_STATE_GAMING);
    }
}
