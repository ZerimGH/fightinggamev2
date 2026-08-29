#include "client.h"
#include "client_private.h"
#include "input.h"
#include "log.h"
#include "packet.h"
#include <stdint.h>

int client_gaming_enter(void) {
    /* Initialise player list */
    for (uint8_t id = 0; id < client.num_players; id++) {
        PlayerInfo *player = &client.players[id];
        player->head       = 0;
        for (uint64_t i = 0; i < MAX_ROLLBACK; i++) {
            player->inputs[i].frame     = (uint64_t)-1;
            player->inputs[i].input.raw = 0;
        }
        player->connected        = 1;
        player->disconnect_frame = (uint64_t)-1;
    }
    return 0;
}

void client_gaming_exit(void) { /* Nothing to do */ }

static void handle_input(uint8_t player_id, TimedInput input) {
    if (player_id >= client.num_players) return;
    PlayerInfo *player = &client.players[player_id];
    if (!player->connected && input.frame > player->disconnect_frame) {
        PERROR(
            "Client received input for disconnected player %d\n", (int)player_id);
        return;
    }

    uint64_t idx = input.frame % MAX_ROLLBACK;
    if (player->inputs[idx].frame == (uint64_t)-1
        || player->inputs[idx].frame < input.frame) {
        player->inputs[idx] = input;
    } else {
        PERROR("Received an out of time input for player %d\n", (int)player_id);
        /* Not sure what to do here? */
        return;
    }
}

static void disconnect_id(uint8_t player_id, uint64_t frame) {
    if (player_id >= client.num_players) return;
    client.players[player_id].connected        = 0;
    client.players[player_id].disconnect_frame = frame;
}

static void handle_packet(ENetEvent *event) {
    Packet packet = packet_parse(event->packet);
    switch (packet.type) {
    case PACKET_SERVER_INPUT:
        handle_input(packet.u.server_input.id, packet.u.server_input.input);
        break;
    case PACKET_SERVER_DISCONNECT:
        disconnect_id(
            packet.u.server_disconnect.id, packet.u.server_disconnect.frame);
        break;
    default:
        PERROR("Client received unhandled packet type %d\n", (int)packet.type);
        /* TODO: Handle error */
        return;
    }
}

void client_gaming_update(void) {
    ENetEvent event;
    while (enet_host_service(client.host, &event, 0) > 0) {
        switch (event.type) {
        case ENET_EVENT_TYPE_DISCONNECT:
            client.peer = NULL;
            client_state_change((ClientState)-1);
            return;
        case ENET_EVENT_TYPE_RECEIVE:
            handle_packet(&event);
            enet_packet_destroy(event.packet);
            break;
        default: break;
        }
    }
}

int client_gaming_get_input(uint8_t player_id, uint64_t frame, Input *input) {
    if (!input || player_id >= client.num_players) return 1;
    PlayerInfo *player = &client.players[player_id];
    if (!player->connected && frame >= player->disconnect_frame) { return 1; }
    uint64_t idx = frame % MAX_ROLLBACK;
    if (player->inputs[idx].frame != frame) return 1;

    *input = player->inputs[idx].input;
    return 0;
}

/* TODO: This function should better validate input */
int client_gaming_send_input(TimedInput input) {
    if (!client.peer) return 1;
    /* Send to server */
    PacketClientInput input_packet;
    input_packet.input = input;
    Packet packet      = packet_create(PACKET_CLIENT_INPUT, &input_packet);
    if (packet.type == PACKET_NULL || packet_send(packet, client.peer)) {
        PERROR("Failed to create client input packet\n");
        /* Not sure how to handle this */
        return 1;
    }
    /* Write to player's local stuff */
    uint64_t idx                                 = input.frame % MAX_ROLLBACK;
    client.players[client.player_id].inputs[idx] = input;
    return 0;
}

int client_gaming_is_connected(uint8_t player_id, uint64_t frame) {
    if (player_id >= client.num_players) return 0;
    PlayerInfo *player = &client.players[player_id];
    if (player->connected) return 1;
    return (frame < player->disconnect_frame);
}

uint8_t client_gaming_get_num_players(void) { return client.num_players; }
