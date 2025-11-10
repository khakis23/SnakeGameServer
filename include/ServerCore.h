//
// Created by Tyler Black on 11/7/25.
//

#ifndef SNAKESERVER_SERVERCORE_H
#define SNAKESERVER_SERVERCORE_H

#include <WSManager.h>
#include "SnakeServer.h"

class Server {
public:
    Server();
    void run();
    void stop();
private:
    WSManager wsm_;
};


Server::Server()
: wsm_{
    [](const std::string& /**/) -> CorePtr {
        return std::make_shared<SnakeServer>();
    }
} {}

void Server::run() {
    wsm_.start();
}

void Server::stop() {
    wsm_.stop();
}



#endif