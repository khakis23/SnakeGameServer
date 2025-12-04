//
// Created by Tyler Black on 11/7/25.
//

#ifndef SNAKESERVER_ICORE_H
#define SNAKESERVER_ICORE_H
#include <string>
#include <memory>


// ABC used by WSManager as an instance factory
class ICore {
    bool user1 = false;
    bool user2 = false;
public:
    virtual ~ICore() = default;

    std::pair<bool, bool> getUsers() { return {user1, user2}; }
    void setUsers(bool connected, int user) { if (user == 1) user1 = connected; else user2 = connected; }
    virtual std::string start() = 0;
    virtual std::string stop() = 0;
    virtual std::string onMessage(std::string_view msg, int seat) = 0;
    virtual void onDisconnect(int seat) = 0;
};

using CorePtr = std::shared_ptr<ICore>;


#endif