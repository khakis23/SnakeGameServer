#ifndef TWOUSERS_WSSERVER_H
#define TWOUSERS_WSSERVER_H

#include <iostream>
#include <unordered_map>
#include <uwebsockets/App.h>
#include <SimpleMsgSender.h>


constexpr int PORT = 9001;
inline constexpr unsigned short IDLE_TIMEOUT = 10;

struct PerSocketData {
    std::string room;
    int seat = -1;   // 0 or 1
    std::weak_ptr<MessageSender> core;
};  // per-connection

using WS = uWS::WebSocket<false, true, PerSocketData>;


class WSManager {
    std::unordered_map<std::string, std::shared_ptr<MessageSender>> rooms;   // active rooms

    void onConnection(auto *&ud, auto &msg, auto &op, auto *&ws);
    void onReconnection(auto *&ud, auto &msg, auto *&ws);
    void startCore(WS* &ws, auto *&ud);
public:
    WSManager();
    bool start();
    void stop();
};


#endif //TWOUSERS_WSSERVER_H