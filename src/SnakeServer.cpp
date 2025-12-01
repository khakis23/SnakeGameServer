#include "SnakeServer.h"


std::string SnakeServer::start() {
    return toJSON(game.getGameCodes());
}


std::string SnakeServer::onMessage(std::string_view msg, int seat) {
    // 1. decode msg
    std::unordered_map<std::string, std::string> msg_map_in;
    std::unordered_map<int, std::string> msg_map_out;

    std::cout << "msg: " << msg << std::endl;

    try {
        msg_map_in = decodeJSON(msg);
    }
    catch (std::runtime_error& e) {
        std::cerr << e.what() << std::endl;
        return "";
    }

    // 2. call game to do something like moveSnake(player=1, Vec2{3, 5})
    for (auto& [key_string, val] : msg_map_in) {
        std::cout << seat << " sent -> " << key_string << " : " << val << std::endl;
        int key = std::stoi(key_string);
        switch (key) {

            case MOVE:
                /*
                 * MOVE : X,Y
                 */
                game.moveSnake(
                    seat,   // player
                    strToVec2(val)            // Vec2
                    );
                // std::cout << "moved: " << strToVec2(val) << std::endl;
                break;

            case READY:
                /*
                 * READY : player
                 */
                ready[seat - 1] = true;
                if (ready[0] && ready[1]) {
                    std::cout << "Game Starting\n";
                    ready[0] = ready[1] = false;   // reset
                    auto codes = game.getGameCodes();
                    codes[SET] = "0";
                    return toJSON(codes);
                }
                break;

            case RESET:
                /*
                 * RESET : 0
                 */
                std::cout << "resetting game\n";
                game.reset();
                break;

            default:
                std::cerr << "unknown key: " << key << std::endl;
                break;
        }
    }

    // game.debugPrint(std::cout);  // TODO temp

    // 3. package any game code (if any) along with the updated player position
    // 4. return a JSON of the package so that it can be published to room
    if (auto send = toJSON(game.getGameCodes()); !send.empty()) {
        std::cout << "send: " << send << '\n';
        return send;
    }
    return "";
}

void SnakeServer::onDisconnect(int seat) {
    std::cout << "Disconnect: " << seat << std::endl;
    game.forceGameOver(seat);
}
