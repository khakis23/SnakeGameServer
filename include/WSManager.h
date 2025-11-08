#ifndef TWOUSERS_WSSERVER_H
#define TWOUSERS_WSSERVER_H

#include <iostream>
#include <unordered_map>
#include <uwebsockets/App.h>
#include "ICore.h"
#include <functional>
#include <memory>
#include <string>


using CoreFactory = std::function<CorePtr(std::string const& roomID)>;

constexpr int PORT = 9001;
inline constexpr unsigned short IDLE_TIMEOUT = 15;

/*** Per Connection ***/
struct PerSocketData {
    std::string room;   // room ID user subscribes to
    int seat = 0;      // 1 or 2
    std::weak_ptr<ICore> core;
};

using WS = uWS::WebSocket<false, true, PerSocketData>;


class WSManager {
    /**
     * Room ID : <shared>* CoreObject
     */
    std::unordered_map<std::string, std::shared_ptr<ICore>> rooms;   // active rooms
    CoreFactory factory_;

    void onConnection(auto *&ud, auto &msg, auto &op, auto *&ws);
    void messageReceived(WS* &ws, std::string_view& msg, auto op);
    void closeConnection(WS* &ws, int code, std::string_view& reason);

    /**
     * @breif Broadcast to room to start Core. Run StartCore logic.
     */
    void startCore(WS* &ws, auto *&ud);
public:
    WSManager(CoreFactory factory);
    bool start();
    bool stop();
};


#endif //TWOUSERS_WSSERVER_H