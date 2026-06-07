#ifndef PACKET_H
#define PACKET_H

#include <stdint.h>

enum { PACKET_START };

typedef uint8_t PacketType;

typedef struct {
    uint8_t id;
    uint8_t num_players;
} __attribute__((packed)) PacketStart;

#endif /* PACKET_H */
