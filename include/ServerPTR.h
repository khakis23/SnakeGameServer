//
// Created by Tyler Black on 11/7/25.
//

#ifndef SNAKESERVER_SERVERCORE_H
#define SNAKESERVER_SERVERCORE_H

#include <WSManager.h>
#include "SnakeServer.h"

/*
 * This class' purpose is to separate WSManager from the rest of the codebase.
 * The simple class holds the start and stop functions of WSManager. More
 * importantly, the class constructures the WSManager using the factory function
 * for the ICore being used. In this program, SnakeSever, is is the derived class
 * of the ICore.
 */

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