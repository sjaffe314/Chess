#include "Game.hpp"
#include "Utils.hpp"

int main(int argc, char *args[])
{
    if (!utils::initSDL())
    {
        return 0;
    }

    Game game;

    game.run();

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}