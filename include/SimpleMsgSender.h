#ifndef SNAKESERVER_SIMPLEMSGSENDER_H
#define SNAKESERVER_SIMPLEMSGSENDER_H
#include <queue>


class MessageSender {
    bool user1 = false;
    bool user2 = false;
public:
    std::pair<bool, bool> getUsers() { return {user1, user2}; }
    void setUsers(bool connected, int user) { if (user == 0) user1 = connected; else user2 = connected; }

    std::string send(const std::string &msg, int to) {return msg;};
};


#endif