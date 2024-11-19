//
// Created by chiendd on 03/10/2024.
//

#ifndef SOCKET_H
#define SOCKET_H
#include <cstring>
#include <sys/socket.h>
#include <unistd.h>


#include "../../util/errors.h"

class Socket {
public:
    Socket() {
        socketNum = socket(AF_INET, SOCK_STREAM, 0);
        if (socketNum < 0) {
            throw Errors("Failed to create socket");
        }
        int opt = 1;
        if (setsockopt(socketNum, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
            throw Errors("Failed to set socket options");
        }
    }

    explicit Socket(int aSocketNum) : socketNum(aSocketNum) {
    };

    ~Socket() { close(socketNum); }

protected:
    int socketNum;
};


#endif //SOCKET_H
