#include "SnakeServer.h"


std::string SnakeServer::start() {
    return toJSON(game.getGameCodes());
}


std::string SnakeServer::onMessage(std::string_view msg) {
    // 1. decode msg
    std::unordered_map<std::string, std::string> msg_map;
    std::cout << "msg: " << msg << std::endl;

    try {
        msg_map = decodeJSON(msg);
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    // get SEAT from msg
    auto iter = msg_map.find(std::to_string(SEAT));
    if (iter == msg_map.end()) {
        std::cerr << "Message missing seat." << std::endl;
        return "";
    }
    std::cout << "seat: " << iter->second << '\n';
    const int seat = stoi(iter->second);
    msg_map.erase(iter);

    // 2. call game to do something like moveSnake(player=1, Vec2{3, 5})
    for (auto& [key, val] : msg_map) {
        std::cout << seat << " sent -> " << key << " : " << val << std::endl;
        switch (stoi(key)) {
            case MOVE:
                /*
                 * Format (string):
                 *      MOVE : player,X,Y
                 */
                game.moveSnake(
                    seat,   // player
                    strToVec2(val)            // Vec2
                    );
                std::cout << "moved: " << strToVec2(val) << std::endl;
                break;

            case RESET:
                // TODO reset game
                break;

            default:
                std::cerr << "unknown key: " << key << std::endl;
                return "";
        }
    }

    game.debugPrint(std::cout);  // TODO temp

    // 3. package any game code (if any) along with the updated player position
    // 4. return a JSON of the package so that it can be published to room
    if (auto send = toJSON(game.getGameCodes()); !send.empty()) {
        std::cout << "send: " << send << '\n';
        return send;
    }
    return "";
}
