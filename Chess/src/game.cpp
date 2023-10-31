#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Pieces.hpp"
#include "Entity.hpp"
#include "Math.hpp"
#include "Utils.hpp"

#include "Game.hpp"

Game::Game()
    : window("Game"), board(window), pieces(window, board) // , gui(window)
{
    // "kQR5/8/8/8/8/8/8/8 b - - 0 10"
    // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
}

void Game::run()
{
    while (gameRunning && !pieces.checkMate) // temporarily just ends program once checkmate
    {
        int startTicks = SDL_GetTicks();

        // updates
        window.updateMousePos(mousePos);
        hovered = board.posToBoard(mousePos);
        pieces.updateSelectedPiecePos(mousePos);

        checkEvents();

        if (change)
            draw();

        int frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < window.getMSPF())
            SDL_Delay(window.getMSPF() - frameTicks);
    }
}

void Game::checkEvents()
{
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_MOUSEBUTTONDOWN:
            pieces.onClick(hovered);
            break;
        case SDL_MOUSEBUTTONUP:
            pieces.onRelease(hovered);
            break;

        case SDL_KEYDOWN:
            switch (event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                gameRunning = false;
                break;
            }
            break;
        case SDL_QUIT:
            gameRunning = false;
            break;
        }
    }
}

void Game::draw()
{
    window.clear();
    board.draw();
    pieces.draw();
    //gui.draw();
    window.display();
}