#ifndef SNAKESERVER_SNAKE_H
#define SNAKESERVER_SNAKE_H

#include <list>
#include <random>
#include "utils.h"
#include "gamecodes.h"


// must match clients' GAME_SIZE
constexpr int GAME_SIZE = 30;


struct Apple {
    Vec2 pos;
};


struct Snake {
    int score = 0;
    int player = 0;      // 1 or 2
    bool grow = false;   // grow flag (growing happens on next frame for server)
    std::list<Vec2> head;
};


/**********************/
/***** GAME CLASS *****/
/**********************/

class Game {
public:

    /*** METHODS ***/

    /**
     * @breif move <player> snake head to coordinate position <to>
     *
     * @param player 1 or 2
     * @param to     (x,y) coordinate
     */
    void moveSnake(int player, const Vec2 &to);

    /**
     * @brief get games codes hash map
     *
     * @return games codes hash map
     */
    std::unordered_map<GameCodes, std::string> getGameCodes();

    /**
     * @brief reset game map (does not reset scores or data)
     */
    void reset();

    /*
     * @brief force a game to end by sending DISCONNECT to clients
     */
    void forceGameOver(int player_num);

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
    void checkCollisionHelper(Snake &a, Snake &b);
    void spawnApple();
    void setupGame();
};


#endif