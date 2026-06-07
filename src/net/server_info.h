#ifndef SERVER_INFO_H
#define SERVER_INFO_H

#include <stdint.h>

#define SERVER_NAME_SIZE 16

typedef struct {
    uint32_t host;
    uint16_t port;
    char name[SERVER_NAME_SIZE];
    uint8_t max_players;
    uint8_t cur_players;

} __attribute__((packed)) ServerInfo;

#endif /* SERVER_INFO_H */
