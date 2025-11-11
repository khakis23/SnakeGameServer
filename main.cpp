#include "ServerCore.h"
#include "SnakeGame.h"  // TODO TESTING
#include <thread>
#include <chrono>

int main() {
    Server server;
    server.run();

    // std::unordered_map<int, std::string> msg = {{MOVE, "1,2"}, {APPLE, "12,7"}};
    // std::string_view msg_str = toJSON(msg);
    // decodeJSON(msg_str);

    // TODO TESTING!!!
    // Game game;
    // int temp = 0;
    // game.debugPrint(std::cout);
    // for (int i = 1; i < 10; ++i) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     game.moveSnake(1, {7, 16+i});
    //     game.debugPrint(std::cout);
    //     temp = 16+i;
    // }
    // int temp2;
    // for (int i = 1; i < 3; ++i) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     game.moveSnake(1, {7 + i, temp});
    //     game.debugPrint(std::cout);
    //     temp2 = 7+i;
    // }
    // int temp3;
    // for (int i = 1; i < 25; ++i) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     game.moveSnake(1, {temp2, temp - i});
    //     game.debugPrint(std::cout);
    //     temp3 = temp - i;
    // }
    // for (int i = 1; i < 10; ++i) {
    //     std::this_thread::sleep_for(std::chrono::milliseconds(500));
    //     game.moveSnake(1, {temp2-i, temp3});
    //     game.debugPrint(std::cout);
    // }

    // std::string_view msg = "{\"msg\":\"hello\"}";
    // auto msg2 = toJSON({"hello", "world", "this", "is", "a", "test"});
    // auto res = decodeJSON<std::string_view>(msg2);


    return 0;
}