//
// Created by chiendd on 03/10/2024.
//

#ifndef SQL_SERVER_H
#define SQL_SERVER_H
#include <condition_variable>
#include <string>

#include "tcp/tcp_server.h"

constexpr int bufSize = 4096;
constexpr int numOfThreads = 5;

class SQLServer {
public:
    SQLServer(const std::string &aHost, unsigned short aPort);

    void run();

    void handleClient(TCPClient *clientSocket);

protected:
    TCPServer tcpServer;
    char *buffer = new char[bufSize];

private:
    std::mutex mtx;
    std::condition_variable cv;
    int active_threads = 0;
};


#endif //SQL_SERVER_H
