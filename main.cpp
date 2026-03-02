#include "engine/Game.hpp"

int main()
{
    Game game;
    if (!game.init())
        return -1;

    game.run();
    game.clean();
    return 0;
}
