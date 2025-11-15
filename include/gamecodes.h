
#ifndef SNAKESERVER_GAMECODES_H
#define SNAKESERVER_GAMECODES_H


// Essential to both server and client
enum GameCodes {
    SEAT,

    // To Client
    START,
    COLLISION,
    APPLE,
    GROW,

    // To Server
    MOVE,
    READY,    // TODO remove?
    RESET,
};


#endif //SNAKESERVER_GAMECODES_H