//
// Created by chiendd on 04/10/2024.
//

#ifndef TCP_SOCKET_H
#define TCP_SOCKET_H
#include "socket.h"
#include <unistd.h>

class TCPClient : public Socket {
public:
    explicit TCPClient(int socketNum) : Socket(socketNum) {
    };

    int read(char *buffer, int size) {
        memset(buffer, 0, size);
        return ::read(socketNum, buffer, size);
    }

    int send(const char *buffer) {
        return ::send(socketNum, buffer, strlen(buffer), 0);
    }
};


#endif //TCP_SOCKET_H
