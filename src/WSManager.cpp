#include "WSManager.h"


WSManager::WSManager() {

}

bool WSManager::start() {
    try {
        uWS::App().ws<PerSocketData>("/*", {
            // keep clients alive
            .idleTimeout = IDLE_TIMEOUT,   // seconds
            .sendPingsAutomatically = true,

            // runs when WS opens
            .open = [&](auto *ws) {
                ws->send("send room id");   // get room authentication
            },

            // first message
            .message = [&](auto *ws, std::string_view msg, uWS::OpCode op) {
                auto *ud = ws->getUserData();

                // first time joining
                if (ud->room.empty()) {
                    onConnection(ud, msg, op, ws);
                    return;
                }

                 if (auto core = ud->core.lock()) {
                     ws->publish(ud->room, core->send(std::string(msg), ud->seat == 0 ? 1 : 0),
                                                                uWS::OpCode::TEXT, false);
                     // TODO might need to ws->send() to sending user due to race conditions
                     std::cout << "sending message to room " << ud->room << "\n";
                 }

            },

            .close = [&](auto *ws, int code, std::string_view /*reason*/) {
                auto *ud = ws->getUserData();
                if (ud->room.empty()) return;

                // safely get the core pointer without creating new room
                const auto& room = ud->room;
                auto iter = rooms.find(room);
                if (iter == rooms.end()) return;
                auto &core = iter->second;

                // remove closing user from room and core
                ws->publish(room, "other disconnected", uWS::OpCode::TEXT, false);
                std::cout << "user " << (ud->seat == 0 ? "1" : "2") << " from room " << room << " disconnected, code: " << code << '\n';
                if (ud->seat == 0)
                    core->setUsers(false, 0);
                else if (ud->seat == 1)
                    core->setUsers(false, 1);
                else return;
                // TODO pause core if necessary

                auto users = core->getUsers();
                if (!users.first && !users.second) {
                    // TODO free core if needed

                    std::cout << "room " << room << " has been closed\n";
                    rooms.erase(iter);
                }
            }
        })
        .listen(PORT, [&](auto *sock) {
            if (!sock) { std::cerr << "listen failed on " << PORT << ".\n"; return; }
            std::cout << "listening on ws://0.0.0.0:" << PORT << "\n";
        })
        .run();
    }
    catch (std::exception &e) {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}


void WSManager::onConnection(auto *&ud, auto &msg, auto &op, auto *&ws) {
    ud->room.assign(msg.data(), msg.size());   // cannot just assign message data
    ws->subscribe(ud->room);

    auto& core = rooms[ud->room];
    // create a core if it doesnt already exist
    if (!core)
        core = std::make_shared<MessageSender>();
    // give the user that core
    ud->core = core;
    auto users = core->getUsers();

    // one user connected, waiting
    if (!users.first && !users.second) {
        std::cout << "room " << ud->room << " has been created\n";
        ws->send("waiting", op, false);
        ud->seat = 0;
        core->setUsers(true, 0);
    }
    // two users connected, start core
    else if (!users.first) {
        ud->seat = 0;
        core->setUsers(true, 0);
        startCore(ws, ud);
    }
    else if (!users.second) {
        ud->seat = 1;
        core->setUsers(true, 1);
        startCore(ws, ud);
    }
    // room full
    else {
        ws->end(1001, "room full");
        std::cout << "room " << ud->room << " is full\n";
    }
    std::cout << "subscribed seat=" << ud->seat << " room=" << ud->room << '\n';
}

void WSManager::startCore(WS* &ws, auto *&ud) {
    // two users connected, start core
    std::cout << "room " << ud->room << " has started\n";
    ws->send("start", uWS::OpCode::TEXT, false);   // direct to this client to fix race condition
    uWS::Loop::get()->defer([ws, ud]() {
        ws->publish(ud->room, "start", uWS::OpCode::TEXT, false);
    });

    // TODO START CORE HERE
}



