#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Pieces.hpp"
#include "Board.hpp"
#include "GUI.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"
#include "Settings.hpp"

class Game
{
public:
    Game();
    void run();

private:
    void checkEvents();
    void draw();

    // Settings settings;
    RenderWindow window;
    bool gameRunning = true;
    bool change = true;
    SDL_Event event;

    Board board;
    Pieces pieces;
    //GUI gui;
    int32_t hovered = -1;
    Vector2i mousePos = {0, 0};
};