#ifndef SNAKESERVER_SNAKE_H
#define SNAKESERVER_SNAKE_H

#include <list>
#include <random>
#include "utils.h"
#include "gamecodes.h"


constexpr int GAME_SIZE = 10;


struct Apple {
    Vec2 pos;
};


struct Snake {
    int player;          // 1 or 2
    bool grow = false;   // grow flag (growing happens on next frame for server)
    std::list<Vec2> head;
    Snake(const std::list<Vec2> &snake, int p) : player(p), head(snake) {}
};


/**********************/
/***** GAME CLASS *****/
/**********************/

class Game {
public:

    /*** METHODS ***/

    /**
     *
     * @param player
     * @param to
     */
    void moveSnake(int player, const Vec2 &to);

    /**
     *
     * @return
     */
    std::unordered_map<GameCodes, std::string> getGameCodes();

    /**
     *
     */
    Game();


    /**
     * Prints server-side game in terminal
     * @param os for printing
     */
    void debugPrint(std::ostream& os) const;

private:

    /*** ATTRIBUTES ***/

    Snake p1;
    Snake p2;
    Apple apple;
    std::unordered_map<GameCodes, std::string> game_codes;
    // for random
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;


    /*** PRIVATE METHODS ***/

    void checkCollision();
    void checkCollisionHelper(Snake &a, const Snake &b);
    void spawnApple();
};


#endif