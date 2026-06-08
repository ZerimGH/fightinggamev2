#include <stdint.h>
#include "client.h"
#include "client_private.h"
#include "log.h"
#include "packet.h"

#define CONNECTION_WAIT_MS 1000

static int connected = 0;
static uint32_t start_time = 0;

int client_waiting_enter(void) {
    if (!client.peer) return 1;

    connected = 0;
    start_time = enet_time_get();
    return 0;
}

void client_waiting_exit(void) {
    connected = 0;
    start_time = 0;
}

void client_waiting_update(void) {
    /* Check for a connection response */
    if (!connected) {
        uint32_t time = enet_time_get();
        if (time - start_time >= CONNECTION_WAIT_MS) {
            PERROR("Failed to connect to server: timeout\n");
            client.peer = NULL;
            client_change_state((ClientState)-1);
            return;
        }
        ENetEvent event;
        if (enet_host_service(client.host, &event, 0) > 0 && event.type == ENET_EVENT_TYPE_CONNECT) {
            connected = 1;
        }
    } else {
        /* Wait for start packet */
        ENetEvent event;
        if (enet_host_service(client.host, &event, 0) > 0) {
            switch (event.type) {
                case ENET_EVENT_TYPE_DISCONNECT:
                    client.peer = NULL;
                    client_change_state((ClientState)-1);
                    return;
                case ENET_EVENT_TYPE_RECEIVE:
                    Packet packet = packet_parse(event.packet);
                    if (packet.type == PACKET_SERVER_START) {
                        client.num_players = packet.u.server_start.num_players;
                        client.player_id = packet.u.server_start.id;
                        if (client_change_state(CS_GAMING)) {
                            PERROR("Failed to enter gaming state\n");
                            client_disconnect();
                            client_change_state((ClientState)-1);
                            return;
                        }
                        return;
                    } else {
                        enet_packet_destroy(event.packet);
                        break;
                    }
                default: break;
            }
        }
    }
}

int client_waiting_connection_confirmed(void) {
    return connected;
}
