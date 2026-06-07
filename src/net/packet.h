#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>
#include <stddef.h>
#include "enet/enet.h"
#include "input.h"

#define INPUT_HIST 5 /* Number of previous frames for the 
                        server to include in input packets */

enum { 
    PACKET_NULL,
    PACKET_SERVER_START,
    PACKET_SERVER_DISCONNECT,
    PACKET_CLIENT_INPUT,
    PACKET_SERVER_INPUT
};

typedef uint8_t PacketType;

typedef struct {
    uint8_t id;
    uint8_t num_players;
} __attribute__((packed)) PacketServerStart;

typedef struct {
    uint8_t id;
} __attribute__((packed)) PacketServerDisconnect;

typedef struct {
    TimedInput input;
    uint8_t id;
} __attribute__((packed)) PacketServerInput;

typedef struct {
    TimedInput input;
} __attribute__((packed)) PacketClientInput;

typedef struct {
    PacketType type;
    union {
        PacketServerStart server_start;
        PacketServerDisconnect server_disconnect;
        PacketServerInput server_input;
        PacketClientInput client_input;
    } u;
    size_t size;
} __attribute__((packed)) Packet;

/* Create a new packet of a type 
 * data is a pointer to the packet struct, for example a 
 * pointer to a PacketServerStart for type PACKET_SERVER_START */
Packet packet_create(PacketType type, void *data);
/* Send a packet to a peer, returns 0 on success */
int packet_send(Packet packet, ENetPeer *peer);
/* Parse a packet from an ENetPacket */
Packet packet_parse(ENetPacket *enet_packet);

#endif /* PACKET_H */
