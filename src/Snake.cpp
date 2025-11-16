#include "Snake.h"
#include <unordered_set>


Game::Game() :
    // init random
    rng(std::random_device{}()),
    dist(0, GAME_SIZE - 1),
    apple() {
    setupGame();   // inits everything for the game
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

void Game::moveSnake(int player, const Vec2 &to) {
    Snake* snake = player == 1 ? &p1 : &p2;
    // std::cout << "player " << player << " moved" << std::endl;

    // move snake
    snake->head.push_front(to);
    if (!snake->grow)
        snake->head.pop_back();   // normal snake movement
    else
        snake->grow = false;      // snake grew last frame, so no need to pop_back()

    // check collision
    checkCollision();
}


void Game::checkCollision() {
    checkCollisionHelper(p1, p2);
    checkCollisionHelper(p2, p1);
}


void Game::checkCollisionHelper(Snake &a, Snake &b) {
    const Vec2* a_head = &a.head.front();

    // check apple collision
    if (*a_head == apple.pos) {
        game_codes[GROW] = std::to_string(a.player);
        a.grow = true;
        spawnApple();
    }

    // wall collision
    if (a_head->x < 0 || a_head->x >= GAME_SIZE
        || a_head->y < 0 || a_head->y >= GAME_SIZE) {
        b.score++;
        game_codes[COLLISION] = std::to_string(a.player);
        return;
    }

    // both heads collide, pick random winner
    if (*a_head == b.head.front()) {
        std::uniform_int_distribution<int> d(1, 2);
        const int loser = d(rng);
        game_codes[COLLISION] = std::to_string(loser);
        if (loser == a.player)
            b.score++;
        else
            a.score++;
        return;
    }

    // self-collision snake bodies
    // A body iteration
    for (auto iter = std::next(p1.head.begin()); iter != p1.head.end(); ++iter) {
        if (*iter == *a_head) {   // A collided with self
            game_codes[COLLISION] = std::to_string(a.player);
            b.score++;
            return;
        }
        if (*iter == b.head.front()) {   // B collided with B body
            game_codes[COLLISION] = std::to_string(b.player);
            a.score++;
            return;
        }
    }
}

std::unordered_map<GameCodes, std::string> Game::getGameCodes() {
    auto temp = game_codes;
    game_codes.clear();
    return temp;
}

void Game::setupGame() {
    // non-first init
    if (!p1.head.empty())
        for (auto* s : {&p1, &p2})
            s->head.clear();
    // first ini
    else {
        p1.player = 1;
        p2.player = 2;
    }

    // create head
    p1.head.push_back({GAME_SIZE / 4, GAME_SIZE / 2});
    p2.head.push_back({3 * GAME_SIZE / 4, GAME_SIZE / 2});

    // add body parts
    for (int i = 1; i < 3; i++) {
        p1.head.push_back({p1.head.front().x, p1.head.front().y + i});
        p2.head.push_back({p2.head.front().x, p2.head.front().y - i});
    }

    spawnApple();
}

void Game::reset() {
    game_codes.clear();
    setupGame();
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

    // clear screen
    #if defined(_WIN32)
            std::system("cls");
    #else
            std::system("clear");
    #endif

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
