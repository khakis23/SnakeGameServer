#include <iostream>
#include <unordered_map>
#include <utility>
#include <uwebsockets/App.h>
#include <uuid/uuid.h>


struct PerSocketData { std::string room; };  // per-connection state

using WS = uWS::WebSocket<false, true, PerSocketData>;


// std::string uuid() {
//     uuid_t id;
//     uuid_generate(id);
//     char buf[37];
//     uuid_unparse(id, buf);
//     return std::string(buf);
// }

std::unordered_map<std::string, std::pair<WS*, WS*>> connections;



int main() {
    int connected = 0;      // how many are currently connected
    bool paired_msg = false; // have we sent the "both connected" message?

    uWS::App()
    .ws<PerSocketData>("/*", {
        // keep clients alive
        .idleTimeout = 10,   // seconds
        .sendPingsAutomatically = true,

        // runs when WS opens
        .open = [&](auto *ws) {
            ws->send("send room id");   // get room authentication
        },

        // echo first message
        .message = [&](auto *ws, std::string_view msg, uWS::OpCode op) {
            auto *ud = ws->getUserData();

            // first time joiining
            if (ud->room.empty()) {
                ud->room.assign(msg.data(), msg.size());   // cannot just assign message data
                auto &usrs = connections[ud->room];

                if (!usrs.first && !usrs.second) {
                    // one user connected, waiting
                    std::cout << "room " << ud->room << " has been created\n";
                    usrs.first = ws;
                    usrs.first->send("waiting for second user", op, false);
                }
                else if (!usrs.first) {
                    // two users connected, start core
                    std::cout << "room " << ud->room << " has started\n";
                    usrs.first = ws;

                    usrs.first->send("start", op, false);
                    usrs.second->send("start", op, false);
                    // TODO START CORE HERE
                }
                else if (!usrs.second) {
                    // two users connected, start core
                     std::cout << "room " << ud->room << " has started\n";
                     usrs.second = ws;

                     usrs.first->send("start", op, false);
                     usrs.second->send("start", op, false);
                     // TODO START CORE HERE
                }
                else {
                    ws->end(1001, "room full");
                }
            }
            // rejoining after disconnection
            else {
                auto it = connections.find(ud->room);
                if (it == connections.end()) return;
                auto &usrs = it->second;
                WS *other = (usrs.first == ws) ? usrs.second : usrs.first;
                if (other) other->send(msg, op, false);
            }
        },

        .close = [&](auto *ws, int /*code*/, std::string_view /*reason*/) {
            auto *ud = ws->getUserData();
            if (ud->room.empty()) return;

            const auto room = ud->room;
            auto iter = connections.find(room);
            if (iter == connections.end()) return;   // this code block is why C++ syntax is gross
            auto &usrs = iter->second;

            WS* other = nullptr;
            if (usrs.second == ws) {   // close user 2
                other = usrs.first;
                usrs.second = nullptr;
            }
            else if (usrs.first == ws) {  // close user 1
                other = usrs.second;
                usrs.first = nullptr;
            }
            else return;

            // update other user
            if (other)
                other->send("other disconnected", uWS::OpCode::TEXT, false);
            std::cout << "a user from room " << room << " has disconnected" << std::endl;

            // TODO pause core if necessary

            if (!usrs.first && !usrs.second) {
                // TODO free core if needed

                std::cout << "room " << room << " has been closed\n";
                connections.erase(iter);
            }
        }
    })
    .listen(9001, [&](auto *sock) {
        if (!sock) { std::cerr << "listen failed on 9001\n"; return; }
        std::cout << "listening on ws://0.0.0.0:9001\n";
    })
    .run();

    return 0;
}