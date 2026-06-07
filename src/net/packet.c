#include "packet.h"
#include <stddef.h>
#include <string.h>

#define DO_MAGIC                                                            \
    X(PACKET_SERVER_START, PacketServerStart)                               \
    X(PACKET_SERVER_DISCONNECT, PacketServerDisconnect)                     \
    X(PACKET_SERVER_INPUT, PacketServerInput)                               \
    X(PACKET_CLIENT_INPUT, PacketClientInput)

static size_t packet_type_size(PacketType type) {
#define X(TYPE, STRUCT) \
    case TYPE: return sizeof(STRUCT);
               switch(type) {
                   DO_MAGIC
                   default: return 0;
               }
#undef X
}

/* This function might be unsafe if used incorrectly */
Packet packet_create(PacketType type, void *data) {
    Packet packet = { .type = PACKET_NULL, .size = 0 };
    size_t size = packet_type_size(type); 
    if (size == 0) {
        return packet; /* Return NULL packet */
    }
    memcpy(&packet.u, data, size);
    packet.type = type;
    packet.size = size + sizeof(PacketType);
    return packet;
}

int packet_send(Packet packet, ENetPeer *peer) {
    ENetPacket *enet_packet = enet_packet_create(&packet, packet.size, ENET_PACKET_FLAG_RELIABLE);

    return enet_peer_send(peer, 0, enet_packet);
}

Packet packet_parse(ENetPacket *enet_packet) {
    Packet packet = packet_create(PACKET_NULL, NULL);
    if (!enet_packet) return packet;
    if (enet_packet->dataLength < sizeof(PacketType)) return packet;

    Packet *cast = (Packet *)enet_packet->data;
    PacketType type = cast->type;
    if (type == PACKET_NULL) return packet;
    size_t type_size = packet_type_size(type);
    size_t expected_size = sizeof(PacketType) + type_size;
    if(enet_packet->dataLength != expected_size) return packet;

    packet.type = type;
    memcpy(&packet.u, &cast->u, expected_size);
    return packet;
}
