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
     * @return game codes
     */
    std::string start() override;

    /**
     * @brief NOT BEING USED
     *
     * @return
     */
    std::string stop() override { return ""; };

    /**
     * @brief controls server-side game by decoding <msg> from player <seat>
     *
     * @param msg  messages received from WebSocket
     * @param seat player number (1 or 2)
     * @return
     */
    std::string onMessage(std::string_view msg, int seat) override;

    /**
     *
     * @param seat
     */
    void onDisconnect(int seat) override;


    /*** NEW METHODS ***/

    // NONE

private:

    /*** ATTRIBUTES ***/

    Game game;
    bool ready[2] = {false, false};
};


#endif //SNAKESERVER_SNAKESERVER_H