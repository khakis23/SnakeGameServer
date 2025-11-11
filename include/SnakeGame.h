//
// Created by Tyler Black on 11/7/25.
//

#ifndef SNAKESERVER_SNAKE_H
#define SNAKESERVER_SNAKE_H

#include "utils.h"
#include <list>
#include <vector>
#include <random>
#include <unordered_set>


constexpr int GAME_SIZE = 30;


struct Apple {
    Vec2 pos;
};


struct SnakeGame {
    std::list<Vec2> head;
    SnakeGame(const std::list<Vec2> &snake) : head(snake) {}
};


// /*
//  * Compare positions of snake & apple using: snake == apple
//  */
// template<typename T>
// bool operator==(T &a, T &b) {
//     return a.x == b.x && a.y == b.y;
// }


enum GameCodes {
    SEAT,

    // To Client
    START,
    COLLISION,
    APPLE,
    GROW,

    // To Server
    MOVE,
    READY,
    RESET,
};

class Game {
public:
    // all public functions must return list a GameCode
    void moveSnake(int player, Vec2 to);
    std::unordered_map<GameCodes, std::string> getGameCodes(int seat);
    Game();

    void debugPrint(std::ostream& os) const;

private:
    SnakeGame p1;
    SnakeGame p2;
    Apple apple;
    // for random
    std::mt19937 rng;
    std::uniform_int_distribution<int> dist;

    std::unordered_map<GameCodes, std::string> game_codes;
    bool game_code_received[2] = {false, false};   // 0 == seat 1, 1 == seat 2

    void checkCollision();
    void spawnApple();
};


Game::Game() :
    // init random
    rng(std::random_device{}()),
    dist(0, GAME_SIZE - 1),
    // init snakes
    p1({Vec2{GAME_SIZE / 4, GAME_SIZE / 2}}),
    p2({Vec2{3 * GAME_SIZE / 4, GAME_SIZE / 2}}) {
    for (int i = 1; i < 3; i++) {
        p1.head.push_back(Vec2{p1.head.front().x, p1.head.front().y + i});
        p2.head.push_back(Vec2{p2.head.front().x, p2.head.front().y - i});
    }

    spawnApple();
}


void Game::spawnApple() {
    Vec2 apple_pos = {dist(rng), dist(rng)};

    auto p1_iter = p1.head.begin();
    auto p2_iter = p2.head.begin();
    std::unordered_set<Vec2> checked;

    // check to make sure apple is not inside a snake body
    while (p1_iter != p1.head.end() || p2_iter != p2.head.end()) {
        bool hit = false;

        if (p1_iter != p1.head.end()) {
            if (*p1_iter == apple_pos)
                hit = true;
            ++p1_iter;
        }
        if (p2_iter != p2.head.end()) {
            if (*p1_iter == apple_pos)
                hit = true;
            ++p2_iter;
        }

        // apple collides with snake, reset
        if (hit) {
            p1_iter = p1.head.begin();
            p2_iter = p2.head.begin();

            // generate new apple position without have to do another check
            checked.insert(apple_pos);
            while (checked.contains(apple_pos))
                apple_pos = {dist(rng), dist(rng)};
        }
    }
    apple.pos = apple_pos;
    game_codes[APPLE] = std::to_string(apple_pos.x) + ',' + std::to_string(apple_pos.y);
}


void Game::moveSnake(int player, Vec2 to) {
    SnakeGame* snake = player == 1 ? &p1 : &p2;
    // std::cout << "player " << player << " moved" << std::endl;

    // move snake
    snake->head.push_front(to);
    snake->head.pop_back();

    // check collision
    checkCollision();
}


void Game::checkCollision() {
    Vec2 p1_head = p1.head.front();
    Vec2 p2_head = p2.head.front();

    // check apple collision
    if (p1_head == apple.pos) {
        game_codes[GROW] = "1";
        spawnApple();
    }
    else if (p2_head == apple.pos) {
        game_codes[GROW] = "2";
        spawnApple();
    }

    // wall collision
    if (p1_head.x < 0 || p1_head.x >= GAME_SIZE
        || p1_head.y < 0 || p1_head.y >= GAME_SIZE) {
        // TODO GAME OVER
        game_codes[COLLISION] = "1";
        return;
    }
    if (p2_head.x < 0 || p2_head.x >= GAME_SIZE
        || p2_head.y < 0 || p2_head.y >= GAME_SIZE) {
        // TODO GAME OVER
        game_codes[COLLISION] = "2";
        return;
    }

    // self-collision with own body (skip head)
    for (auto iter = std::next(p1.head.begin()); iter != p1.head.end(); ++iter) {
        if (*iter == p1_head) {
            // TODO GAME OVER
            game_codes[COLLISION] = "1";
            return;
        }
    }
    for (auto iter = std::next(p2.head.begin()); iter != p2.head.end(); ++iter) {
        if (*iter == p2_head) {
            // TODO GAME OVER
            game_codes[COLLISION] = "2";
            return;
        }
    }

    // collision with other snake
    for (int i = 1; i <= 2; i++) {
        std::list<Vec2>::iterator iter, end;
        Vec2 other;
        int other_i;

        if (i == 1) {
            iter = p1.head.begin();
            end = p1.head.end();
            other = p1_head;
            other_i = 2;
        }
        else {
            iter = p2.head.begin();
            end = p2.head.end();
            other = p2_head;
            other_i = 1;
        }

        while (iter != end) {
            if (*iter == other) {
                // TODO GAME OVER
                game_codes[COLLISION] = std::to_string(other_i);
                return;
            }
            ++iter;
        }
    }
}


void Game::debugPrint(std::ostream& os) const {
    /*
     * Chat wrote most of this method, no credit to me if ever breaks...
     */
    constexpr const char* EMPTY  = " ";
    constexpr const char* S1BODY = "▓";
    constexpr const char* S1HEAD = "█";
    constexpr const char* S2BODY = "▒";
    constexpr const char* S2HEAD = "█";   // same head glyph
    constexpr const char* APPLE  = "●";

    system("clear");

    std::vector<std::vector<const char*>> board(
        GAME_SIZE, std::vector<const char*>(GAME_SIZE, EMPTY)
    );

    // p1
    if (!p1.head.empty()) {
        for (auto it = std::next(p1.head.begin()); it != p1.head.end(); ++it) {
            const Vec2& v = *it;
            if (v.x>=0 && v.x<GAME_SIZE && v.y>=0 && v.y<GAME_SIZE)
                board[v.y][v.x] = S1BODY;
        }
        Vec2 h = p1.head.front();
        if (h.x>=0 && h.x<GAME_SIZE && h.y>=0 && h.y<GAME_SIZE)
            board[h.y][h.x] = S1HEAD;
    }

    // p2
    if (!p2.head.empty()) {
        for (auto it = std::next(p2.head.begin()); it != p2.head.end(); ++it) {
            const Vec2& v = *it;
            if (v.x>=0 && v.x<GAME_SIZE && v.y>=0 && v.y<GAME_SIZE)
                board[v.y][v.x] = S2BODY;
        }
        Vec2 h = p2.head.front();
        if (h.x>=0 && h.x<GAME_SIZE && h.y>=0 && h.y<GAME_SIZE)
            board[h.y][h.x] = S2HEAD;
    }

    // apple
    if (apple.pos.x>=0 && apple.pos.x<GAME_SIZE &&
        apple.pos.y>=0 && apple.pos.y<GAME_SIZE) {
        board[apple.pos.y][apple.pos.x] = APPLE;
        }

    // top border (double-wide)
    os << "┌";
    for (int x = 0; x < GAME_SIZE; ++x) os << "──";
    os << "┐\n";

    // rows
    for (int y = 0; y < GAME_SIZE; ++y) {
        os << "│";
        for (int x = 0; x < GAME_SIZE; ++x)
            os << board[y][x] << board[y][x];
        os << "│\n";
    }

    // bottom border
    os << "└";
    for (int x = 0; x < GAME_SIZE; ++x) os << "──";
    os << "┘\n";

    // os << "Legend: " << S1HEAD << "/"<< S1BODY << " P1  "
    //    << S2HEAD << "/"<< S2BODY << " P2  "
    //    << APPLE << " Apple\n";
}


std::unordered_map<GameCodes, std::string> Game::getGameCodes(int seat) {
    // both clients received last message, reset
    if (game_code_received[0] && game_code_received[1]) {
        game_code_received[0] = game_code_received[1] = false;
        game_codes.clear();
    }
    game_code_received[seat - 1] = true;

    auto codes = game_codes;
    codes[SEAT] = std::to_string(seat);

    return game_codes;
}


#endif