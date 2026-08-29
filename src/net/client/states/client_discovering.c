/* Mostly copied line for line from https://github.com/cxong/ENetLANChatServer */

#include "client_discovering.h"
#include "client.h"
#include "client_private.h"
#include "discovery.h"
#include "enet/enet.h"
#include "server_info.h"
#include <string.h>

#define MAX_SERVERS 64
#define SEND_RATE 1000
#define MAX_AGE 5000

static ENetSocket sock  = ENET_SOCKET_NULL;
static ENetAddress addr = {0};

static ServerInfo infos[MAX_SERVERS] = {{0}};
static int num_infos                 = 0;
static uint32_t times[MAX_SERVERS]   = {0};

static uint32_t last_send = 0;

int client_discovering_enter(void) {
    /* Set up discovery stuff */
    /* Create a socket for discovery */
    sock = enet_socket_create(ENET_SOCKET_TYPE_DATAGRAM);
    if (sock == ENET_SOCKET_NULL) return 1;
    if (enet_socket_set_option(sock, ENET_SOCKOPT_BROADCAST, 1)) {
        enet_socket_destroy(sock);
        sock = ENET_SOCKET_NULL;
        return 1;
    }

    addr.host = ENET_HOST_BROADCAST;
    addr.port = DISCOVERY_PORT;

    num_infos = 0;
    last_send = 0;
    return 0;
}

void client_discovering_exit(void) {
    if (sock != ENET_SOCKET_NULL) enet_socket_destroy(sock);
    sock = ENET_SOCKET_NULL;
}

int client_discovering_get_servers(ServerInfo *buf, int buf_size) {
    int count = (num_infos < buf_size) ? num_infos : buf_size;
    memcpy(buf, infos, (size_t)count * sizeof(ServerInfo));
    return count;
}

void client_discovering_update(void) {
    uint32_t current_time = enet_time_get();

    /* Do discovery */
    /* Send data to discovery port */
    if (current_time - last_send >= SEND_RATE) {
        char data = 69;
        ENetBuffer sendbuf;
        sendbuf.data       = &data;
        sendbuf.dataLength = sizeof(data);
        if (enet_socket_send(sock, &addr, &sendbuf, 1) != (int)sizeof(data))
            return;
        last_send = current_time;
    }

    /* Listen for replies */
    ENetSocketSet set;
    ENET_SOCKETSET_EMPTY(set);
    ENET_SOCKETSET_ADD(set, sock);

    if (enet_socketset_select(sock, &set, NULL, 0) > 0) {
        ENetAddress reply_addr;
        ServerInfo new_info;
        ENetBuffer recvbuf;

        recvbuf.data       = &new_info;
        recvbuf.dataLength = sizeof(new_info);

        int recvlen = enet_socket_receive(sock, &reply_addr, &recvbuf, 1);
        if (recvlen == sizeof(ServerInfo)) {
            new_info.host = reply_addr.host;

            int found = 0;
            for (int i = 0; i < num_infos; i++) {
                if (infos[i].host == new_info.host
                    && infos[i].port == new_info.port) {
                    infos[i].cur_players = new_info.cur_players;
                    infos[i].max_players = new_info.max_players;
                    times[i]             = current_time;
                    found                = 1;
                    break;
                }
            }

            if (!found && num_infos < MAX_SERVERS) {
                times[num_infos]   = current_time;
                infos[num_infos++] = new_info;
            }
        }
    }

    for (int i = 0; i < num_infos; i++) {
        if (current_time - times[i] > MAX_AGE) {
            infos[i] = infos[num_infos - 1];
            times[i] = times[num_infos - 1];
            num_infos--;
            i--;
        }
    }
}
