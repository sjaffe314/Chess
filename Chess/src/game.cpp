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
        case GUIEvents::NewGame:
            pieces.newGame();
            gui.clearTexts();
            gui.resetClocks();
            board.clearAllHighlights();
            break;
        case GUIEvents::Quit:
            gameRunning = false;
            break;
        case GUIEvents::StopGame:
            pieces.allowNoMoves();
            break;
        case GUIEvents::ContinueGame:
            pieces.calculateCurrentLegalMoves();
            break;
        }
    }
    while (myEvent = pieces.pollEvent())
    {
        switch (myEvent)
        {
        case PiecesEvents::ChangedTurn:
            gui.toggleTurn();
            break;
        case PiecesEvents::WhiteWin:
            gui.show(TextIndicies::Checkmate);
            gui.show(TextIndicies::WhiteWins);
            break;
        case PiecesEvents::BlackWin:
            gui.show(TextIndicies::Checkmate);
            gui.show(TextIndicies::BlackWins);
            break;
        case PiecesEvents::Stalemate:
            gui.show(TextIndicies::Stalemate);
            gui.show(TextIndicies::Tied);
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