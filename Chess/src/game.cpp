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
    : window(RenderWindow::get()), board(), pieces(board), gui()
{
    // "kQR5/8/8/8/8/8/8/8 b - - 0 10"
    // "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
}

void Game::run()
{
    while (gameRunning) // temporarily just ends program once checkmate
    {
        int startTicks = SDL_GetTicks();

        // updates
        window.updateMousePos(mousePos);
        hovered = board.posToBoard(mousePos);
        pieces.updateSelectedPiecePos(mousePos);
        gui.update(mousePos);

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
            gui.onMouseDown(mousePos);
            pieces.onClick(hovered);
            break;
        case SDL_MOUSEBUTTONUP:
            gui.onMouseUp(mousePos);
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
    while (myEvent = gui.pollEvent())
    {
        switch (myEvent)
        {
        case NewGame:
            pieces.newGame();
            gui.resetClocks();
            board.clearAllHighlights();
            break;
        case Quit:
            gameRunning = false;
            break;
        }
    }
    while (myEvent = pieces.pollEvent())
    {
        switch (myEvent)
        {
        case ChangedTurn:
            gui.toggleTurn();
            break;
        case WhiteWin:
            gui.showCheckmate();
            break;
        case BlackWin:
            gui.showCheckmate();
            break;
        case Stalemate:
            break;
        }
    }
}

void Game::draw()
{
    window.clear();
    board.draw();
    pieces.draw();
    gui.draw();
    window.display();
}