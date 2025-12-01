
#ifndef SNAKESERVER_GAMECODES_H
#define SNAKESERVER_GAMECODES_H


// Essential to both server and client
enum GameCodes {
    // To Client    - KEYS -
    SEAT,        //
    START,       //  player
    COLLISION,   //  player
    APPLE,       //  x,y
    GROW,        //  player
    SCORE,       //  s1,s2
    SET,         //  NULL
    DISCONNECT,  //  player

    // both
    MOVE,        //  x,y

    // To Server
    READY,      //  NULL
    RESET,      //  NULL
};



#endif //SNAKESERVER_GAMECODES_H