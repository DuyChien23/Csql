//
// Created by chiendd on 03/10/2024.
//

#ifndef TCP_SERVER_H
#define TCP_SERVER_H
#include <arpa/inet.h>
#include <netinet/in.h>

#include "socket.h"
#include "tcp_client.h"
#include "../../util/configs.h"

class TCPServer : public Socket {
public:
    TCPServer(const std::string &aHost, unsigned short aPort) {
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = inet_addr(aHost.c_str());
        address.sin_port = htons(aPort);
    }

    void execute() {
        if (bind(socketNum, (sockaddr *) &address, sizeof(address)) < 0) {
            throw Errors("Failed to bind socket");
        }
        if (listen(socketNum, Configs::maxBacklog) < 0) {
            throw Errors("Failed to listen on socket");
        }
    }

    TCPClient *acceptConnection() {
        int addrlen = sizeof(address);
        int newSocket = accept(socketNum, (sockaddr *) &address, (socklen_t *) &addrlen);
        if (newSocket < 0) {
            throw Errors("Failed to accept connection");
        }
        return new TCPClient(newSocket);
    }

private:
    sockaddr_in address;
};


#endif //TCP_SERVER_H
