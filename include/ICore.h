//
// Created by Tyler Black on 11/7/25.
//

#ifndef TWOUSERS_ICORE_H
#define TWOUSERS_ICORE_H
#include <string>


class ICore {
    bool user1 = false;
    bool user2 = false;
public:
    virtual ~ICore() = default;

    std::pair<bool, bool> getUsers() { return {user1, user2}; }
    void setUsers(bool connected, int user) { if (user == 0) user1 = connected; else user2 = connected; }
    virtual void start() = 0;
    virtual void stop() = 0;
    virtual std::string onMessage(int seat, std::string_view msg) = 0;
    virtual void onDisconnect(int seat) = 0;
};

using CorePtr = std::shared_ptr<ICore>;


#endif //TWOUSERS_ICORE_H