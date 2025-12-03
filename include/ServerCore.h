//
// Created by Tyler Black on 11/7/25.
//

#ifndef SNAKESERVER_SERVERCORE_H
#define SNAKESERVER_SERVERCORE_H

#include <WSManager.h>
#include "SnakeServer.h"


// TODO DOCS: this keeps things modular
class Server {
public:
    Server();
    void run();
    void stop();
private:
    WSManager wsm;
};


inline Server::Server()
: wsm{
    [](const std::string& /**/) -> CorePtr {
        return std::make_shared<SnakeServer>();
    }
} {}

inline void Server::run() {
    wsm.start();
}

inline void Server::stop() {
    wsm.stop();
}


#endif