#ifndef SNAKESERVER_SNAKESERVER_H
#define SNAKESERVER_SNAKESERVER_H
#include "ICore.h"
#include <string>
#include "SnakeGame.h"


class SnakeServer : public ICore {
public:
    SnakeServer() = default;
    void start() override {};
    void stop() override {};
    std::string onMessage(int seat, std::string_view msg) override;
    void onDisconnect(int seat) override {};

    void printDebug() const {};

private:
    Game game;
};


std::string SnakeServer::onMessage(int seat, std::string_view msg) {
    // 1. decode msg
    std::unordered_map<std::string, std::string> msg_map;

    try {
        msg_map = decodeJSON(msg);
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    // 2. call game to do something like moveSnake(player=1, Vec2{3, 5})
    for (auto& [key, val] : msg_map) {
        std::cout << seat << " sent -> " << key << " : " << val << std::endl;
        switch (int key_enum = stoi(key)) {   // TODO parsing issue, ALSO dont need to store player, just use seat
            case MOVE:
                /*
                 * Format (string):
                 *      MOVE : player,X,Y
                 */
                game.moveSnake(
                    std::stoi(val.substr(0, 1)),   // player
                    strToVec2(val.substr(2))            // Vec2
                    );
                break;

            default:
                std::cerr << "unknown key: " << key << std::endl;
                return "";
        }
    }

    // 3. package any game code (if any) along with the updated player position
    auto send = toJSON(game.getGameCodes());

    // 4. return a JSON of the package so that it can be published to room
    if (!send.empty())
        return send;
    return "";
}


#endif //TWOUSERS_SNAKESERVER_H