//
// Created by chiendd on 03/10/2024.
//

#include "sql_server.h"

#include <thread>

#include "../controller/database_controller.h"
#include "../controller/main_controller.h"
#include "../controller/tokenizer/tokenizer.h"

SQLServer::SQLServer(const std::string &aHost, unsigned short aPort) : tcpServer(aHost, aPort) {
};

void SQLServer::run() {
    tcpServer.execute();
    while (true) {
        TCPClient *clientSocket = tcpServer.acceptConnection();

        std::unique_lock lock(mtx);
        cv.wait(lock, [this] { return active_threads < numOfThreads; });
        active_threads++;
        lock.unlock();

        std::thread t([this, clientSocket] {
            handleClient(clientSocket);

            std::lock_guard lock(mtx);
            active_threads--;
            cv.notify_one();
        });

        t.detach();
    }
}

void SQLServer::handleClient(TCPClient *clientSocket) {
    char *buffer = new char[bufSize];

    while (clientSocket->read(buffer, bufSize) > 0) {
        try {
            std::istringstream iss(buffer);
            Tokenizer tokenizer(iss);
            tokenizer.tokenize();

            std::stringstream theOutput;
            MainController mainController(theOutput);
            Statement *statement = mainController.parser(tokenizer);
            if (statement == nullptr) {
                clientSocket->send("Failed to parse the statement");
            } else {
                statement->execute();
                clientSocket->send(mainController.getOutput().c_str());
            }
        } catch (Errors &e) {
            clientSocket->send(e.what());
        }
    }

    DatabaseController::closeDatabase();
    delete []buffer;
    delete clientSocket;
}
