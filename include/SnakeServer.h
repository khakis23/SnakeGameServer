#ifndef SNAKESERVER_SNAKESERVER_H
#define SNAKESERVER_SNAKESERVER_H
#include "ICore.h"
#include <string>
#include "Snake.h"


class SnakeServer : public ICore {
public:

    /*** ICORE OVERRIDES ***/

    SnakeServer() = default;

    /**
     *
     * @return
     */
    std::string start() override;

    /**
     *
     * @return
     */
    std::string stop() override { return ""; };

    /**
     *
     * @param msg
     * @param seat
     * @return
     */
    std::string onMessage(std::string_view msg, int seat) override;

    /**
     *
     * @param seat
     */
    void onDisconnect(int seat) override {};


    /*** NEW METHODS ***/

    // NONE

private:

    /*** ATTRIBUTES ***/

    Game game;
};


#endif //SNAKESERVER_SNAKESERVER_H