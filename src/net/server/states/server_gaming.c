#include "server_gaming.h"

#include <stdint.h>
#include "server.h"
#include "server_private.h"
#include "enet/enet.h"
#include "log.h"
#include "packet.h"
#include "global.h"

#define MAX_AHEAD 32
#define MAX_BEHIND 32

#define FPS 60 

static uint32_t start_time = 0;

int server_gaming_enter(void) {
    /* Verify all clients are contiguous */
    for (uint8_t id = 0; id < server.num_clients; id++) {
        ClientInfo *client = &server.clients[id];
        if (!client->peer) {
            PERROR("Client %d peer is NULL, connected clients must be"
                    " contiguous in memory\n", (int)id);
            return 1;
        }
    }

    /* Send start packet to all clients */
    PacketServerStart start_packet;
    start_packet.num_players = server.num_clients;

    for (uint8_t id = 0; id < server.num_clients; id++) {
        ClientInfo *client = &server.clients[id];
        start_packet.id = client->player_id;
        Packet packet = packet_create(PACKET_SERVER_START, &start_packet);
        if (packet.type == PACKET_NULL) {
            PERROR("Failed to create start packet\n");
            return 1;
        }
        if (packet_send(packet, client->peer)) {
            PERROR("Failed to send start packet to client %d\n",
                    (int)client->player_id);
            return 1;
        }
    }

    start_time = enet_time_get();
    return 0;
}

void server_gaming_exit() {
    /* Disconnect all clients */
    for (uint8_t id = 0; id < server.num_clients; id++) {
        enet_peer_disconnect_now(server.clients[id].peer, 0);
        server.clients[id].peer = NULL;
    }
    start_time = 0;
}

static void disconnect_id(uint8_t player_id) {
    if (player_id >= server.num_clients) return;
    /* Disconnect the peer */
    ClientInfo *client = &server.clients[player_id];
    if (!client->peer) {
        PERROR("(warn) disconnect_id() called on a disconnected client\n");
        return;
    }
    enet_peer_disconnect_now(client->peer, 0);
    client->peer = NULL;

    /* Tell other clients about disconnection */
    /* TODO: Clients will receive this message at different times,
     * I think the server should predict the disconnected player's inputs up
     * to the furthest ahead client's most recent input to ensure 
     * synchronisation */
    PacketServerDisconnect disconnect_packet;
    disconnect_packet.id = player_id;
    Packet packet = packet_create(PACKET_SERVER_DISCONNECT, &disconnect_packet);
    if (packet.type == PACKET_NULL) {
        PERROR("Failed to create disconnection packet for client %d\n", 
                (int)player_id);
        /* TODO: Handle error */
        return;
    }

    for (uint8_t id = 0; id < server.num_clients; id++) {
        if (id == player_id) continue;
        ClientInfo *other_client = &server.clients[id];
        if (!other_client->peer) continue;
        if (packet_send(packet, other_client->peer)) {
            PERROR("Failed to send disconnect packet for client %d"
                    " to client %d\n",
                    (int)player_id, (int)id);
            /* TODO: Handle error */
            continue;
        }
    }
}

static void handle_connect(ENetEvent *event) {
    /* Don't allow connections while game running */
    enet_peer_disconnect_now(event->peer, 0);
}

static void handle_disconnect(ENetEvent *event) {
    ClientInfo *client = event->peer->data;
    uint8_t id = client->player_id;
    disconnect_id(id);
}

static void handle_client_input(ClientInfo *client, TimedInput input) {
    /* TODO: Out of order inputs should not be an error, 
     * but idk how to handle them right now. */

    /* Validate input */
    if (input.frame != client->latest_input + 1) {
        PINFO("Client %d sent out of order input, "
                "expected: %llu, sent: %llu, disconnecting\n",
                (unsigned long long)client->latest_input + 1,
                (unsigned long long)input.frame);
        /* I guess disconnect the client? */
        disconnect_id(client->player_id);
    }
    client->latest_input++;

    PacketServerInput input_packet;
    input_packet.input = input;
    input_packet.id = client->player_id;
    Packet packet = packet_create(PACKET_SERVER_INPUT, &input_packet);
    if (packet.type == PACKET_NULL) {
        PERROR("Failed to create server input packet for client %d on frame %llu\n");
        /* TODO: Handle error */
        return;
    }

    /* Relay input to other clients */
    for (uint8_t id = 0; id < server.num_clients; id++) {
        if(id == client->player_id) continue;
        ClientInfo *other_client = &server.clients[id];
        if (!other_client->peer) continue;
        if (packet_send(packet, other_client->peer)) {
            PERROR("Failed to send input for client %d to client %d\n",
                    (int)client->player_id, (int)other_client->player_id);
            /* TODO: Handle error*/
            continue;
        }
    }
}

static void handle_packet(ENetEvent *event) {
    Packet packet = packet_parse(event->packet);    
    switch (packet.type) {
        case PACKET_CLIENT_INPUT:
            handle_client_input(event->peer->data,
                    packet.u.client_input.input);
            break;
        default:
            PERROR("Server received unhandled packet type %d\n",
                    (int)packet.type);
            /* TODO: Handle error */
            return;
    }
}

void server_gaming_update() {
    uint32_t cur_time = enet_time_get();
    uint32_t game_time = cur_time - start_time;
    uint64_t expected_frame = (uint64_t)((float)game_time / 1000.f * (float)FPS);
    /* Handle packets from clients */
    ENetEvent event;
    while (enet_host_service(server.host, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT: handle_connect(&event); break;
            case ENET_EVENT_TYPE_DISCONNECT: handle_disconnect(&event); break;
            case ENET_EVENT_TYPE_RECEIVE:
                                             handle_packet(&event); 
                                             enet_packet_destroy(event.packet);
                                             break;
            default: break;
        }
    }

    /* Handle prediction and disconnection logic and stuff */
    /* For now, just disconnect all clients too far behind or ahead */
    for (uint8_t id = 0; id < server.num_clients; id++) {
        ClientInfo *client = &server.clients[id];
        if (!client->peer) continue;
        uint64_t frame = client->latest_input;
        if (frame == (uint64_t)-1) frame = 0;
        if (frame >= expected_frame) {
            uint64_t ahead = frame - expected_frame;
            if (ahead > MAX_AHEAD) {
                disconnect_id(id);
                PINFO("Disconnected player %d, inputs were too far ahead\n",
                        (int)id);
            }
        } else {
            uint64_t behind = expected_frame - frame;
            if (behind > MAX_BEHIND) {
                disconnect_id(id);
                PINFO("Disconnected player %d, inputs were too far behind\n",
                        (int)id);
            }
        }
    }
}

uint8_t server_gaming_count_clients(void) {
    uint8_t count = 0;
    for (uint8_t i = 0; i < server.num_clients; i++) {
        ClientInfo *client = &server.clients[i];
        if (client->peer) count++;
    }

    return count;
}
