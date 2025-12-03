/*
 *  HOW TO CONNECT A CLIENT:
 *      1. Establish WS connection on IP:PORT. Upon connection, each client must
 *          respond <TEXT>"<room name/id>".
 *      2. Once both users have been accepted in a room, the server will send
 *          <JSON>{"start": <seat (1 or 2)>}.
 *      3. The core will begin. If another client tries to join the same room,
 *          the connection will be ended with 1001, <TEXT>"room full"
 */

#ifndef SNAKESERVER_WSSERVER_H
#define SNAKESERVER_WSSERVER_H

#include <unordered_map>
#include <uwebsockets/App.h>
#include "ICore.h"
#include <functional>
#include <memory>
#include <string>


constexpr int PORT = 9001;
inline constexpr unsigned short IDLE_TIMEOUT = 15;


/*** Per Connection ***/
struct PerSocketData {
    std::string room;   // room ID user subscribes to
    int seat = 0;       // 1 or 2
    std::weak_ptr<ICore> core;
};

using WS = uWS::WebSocket<false, true, PerSocketData>;
using CoreFactory = std::function<CorePtr(std::string const& roomID)>;   // ICore


class WSManager {
public:
    WSManager(CoreFactory factory);
    bool start();
    bool stop();

private:

    /**
     * @brief when a client connects -> assigns user number, roomID, assigns core pointer or instantiates Core.
     */
    void onConnection(auto *&ud, auto &msg, auto &op, auto *&ws);

    /**
     * @brief Handles all messages (send and receive) from client.
     *
     * @param ws WebSocket Object from uWebsockets
     * @param msg Message from uWebsockets
     * @param op Op Code from uWebsockets
     */
    void messageReceived(WS* &ws, std::string_view& msg, auto op);

    /**
     *
     * @param ws WebSocket Object from uWebsockets
     * @param code Exit/disconnect code
     * @param reason String reason for exit/disconnect
     */
    void closeConnection(WS* &ws, int code, std::string_view& reason);

    /**
     * @breif Broadcast to room to start Core. Run StartCore logic.
     */
    void startCore(WS* &ws, auto *&ud);


    // Room ID : <shared>* CoreObject
    std::unordered_map<std::string, std::shared_ptr<ICore>> rooms;   // active rooms
    CoreFactory factory;
};


#endif