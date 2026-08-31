#include "client.h"
#include "client_private.h"
#include "log.h"
#include "packet.h"
#include <stdint.h>

#define CONNECTION_WAIT_MS 1000

static int connected       = 0;
static uint32_t start_time = 0;

int client_waiting_enter(void) {
    if (!client.peer) return 1;

    connected  = 0;
    start_time = enet_time_get();
    return 0;
}

void client_waiting_exit(void) {
    connected  = 0;
    start_time = 0;
}

static void handle_disconnect(ENetEvent event) {
    client.peer = NULL;
    client_state_change((ClientState)-1);
}

static void handle_receive(ENetEvent event) {
    Packet packet = packet_parse(event.packet);
    if (packet.type == PACKET_SERVER_START) {
        client.num_players = packet.u.server_start.num_players;
        client.player_id   = packet.u.server_start.id;
        client.delay       = packet.u.server_start.delay;
        if (client_state_change(CLIENT_STATE_GAMING)) {
            PERROR("Failed to enter gaming state\n");
            client_disconnect();
            client_state_change((ClientState)-1);
        }
    } else if (packet.type == PACKET_SERVER_SYNC) {
        PacketClientSync client_sync = {0};
        Packet packet = packet_create(PACKET_CLIENT_SYNC, &client_sync);
        if (packet_send(packet, client.peer)) { /* TODO: HANDLE ERROR */
        }
    }
    enet_packet_destroy(event.packet);
}

void client_waiting_update(void) {
    /* Check for a connection response */
    if (!connected) {
        uint32_t time = enet_time_get();
        if (time - start_time >= CONNECTION_WAIT_MS) {
            PERROR("Failed to connect to server: timeout\n");
            client.peer = NULL;
            client_state_change((ClientState)-1);
            return;
        }
        ENetEvent event;
        if (enet_host_service(client.host, &event, 0) > 0
            && event.type == ENET_EVENT_TYPE_CONNECT) {
            connected = 1;
        }
    } else {
        /* Wait for start packet */
        ENetEvent event;
        if (enet_host_service(client.host, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_DISCONNECT: handle_disconnect(event); return;
            case ENET_EVENT_TYPE_RECEIVE:    handle_receive(event);
            default:                         break;
            }
        }
    }
}

int client_waiting_connection_confirmed(void) { return connected; }
