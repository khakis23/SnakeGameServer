#include "WSManager.h"


WSManager::WSManager(CoreFactory factory) : factory_(std::move(factory)) {}

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
                messageReceived(ws, msg, op);
            },

            .close = [&](auto *ws, int code, std::string_view reason) {
                closeConnection(ws, code, reason);
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

bool WSManager::stop() {
    // TODO
    return true;
}


void WSManager::messageReceived(WS *&ws, std::string_view &msg, auto op) {
    auto *ud = ws->getUserData();

    // user joined or rejoined
    if (ud->room.empty()) {
        onConnection(ud, msg, op, ws);
        return;
    }

    // normal message path
    if (auto core = ud->core.lock()) {
        // send message to core, returns its message
        const std::string out_going = core->onMessage(ud->seat, msg);

        // publish the outgoing JSON message to all users
        ws->publish(ud->room, out_going, uWS::OpCode::TEXT, false);
        // TODO might need to ws->send() to sending user due to race conditions...NVM isn't that what ud->seat does??
        // std::cout << "sending message to room " << ud->room << "\n";
    }
}


void WSManager::onConnection(auto *&ud, auto &msg, auto &op, auto *&ws) {
    ud->room.assign(msg.data(), msg.size());   // cannot just assign message data
    ws->subscribe(ud->room);

    auto& core = rooms[ud->room];
    // create a core if it doesnt already exist
    if (!core) {
        core = factory_(ud->room);
        if (!core) {
            ws->end(1011, "Core factory could not instantiate.");
            return;
        }
    }

    ud->core = core;  // give the user access to core pointer
    auto users = core->getUsers();

    // one user connected, waiting
    if (!users.first && !users.second) {
        std::cout << "room " << ud->room << " has been created\n";
        // ws->send("waiting", op, false);
        ud->seat = 1;
        core->setUsers(true, 1);
    }
    // both users connected, start core
    else if (!users.first) {
        ud->seat = 1;
        core->setUsers(true, 1);
        startCore(ws, ud);
    }
    else if (!users.second) {
        ud->seat = 2;
        core->setUsers(true, 2);
        startCore(ws, ud);
    }
    // room full case
    else {
        ws->end(1001, "room full");
        std::cout << "room " << ud->room << " is full\n";
    }

    std::cout << "subscribed seat=" << ud->seat << " room=" << ud->room << '\n';
}

void WSManager::startCore(WS* &ws, auto *&ud) {
    // two users connected
    std::cout << "room " << ud->room << " has started\n";
    std::string const json_ws = "{\"start\":" + std::to_string(ud->seat) + "}";
    std::string const json_other = "{\"start\":" + std::string(ud->seat == 1 ? "2" : "1") + "}";
    ws->send(json_ws, uWS::OpCode::TEXT, false);   // direct to this client to fix race condition
    uWS::Loop::get()->defer([ws, ud, json_other]() {
        ws->publish(ud->room, json_other, uWS::OpCode::TEXT, false);
    });

    // start Core
    rooms[ud->room]->start();
}


void WSManager::closeConnection(WS* &ws, int code, std::string_view &reason) {
    auto *ud = ws->getUserData();
    if (ud->room.empty()) return;

    // safely get the core pointer without creating new room
    const auto& room = ud->room;
    auto iter = rooms.find(room);
    if (iter == rooms.end()) return;
    auto &core = iter->second;

    // user disconnected
    ws->publish(room, "other disconnected", uWS::OpCode::TEXT, false);
    std::cout << "user " << (ud->seat == 1 ? "1" : "2") << " from room " << room << " disconnected, code: " << code << '\n';
    core->setUsers(false, ud->seat);
    core->onDisconnect(ud->seat);

    // both users hva ebeen disconnected
    auto users = core->getUsers();
    if (!users.first && !users.second) {
        core->stop();

        std::cout << "room " << room << " has been closed\n";
        rooms.erase(iter);
    }
}


