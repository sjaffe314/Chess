#pragma once

#include <SDL.h>

#include "RenderWindow.hpp"
#include "Utils.hpp"
#include "Math.hpp"

class Board
{
public:
    Board(RenderWindow& p_window);
    void updateBoardTexture();

    void draw();
    void drawHover() { window.drawHoverInd(boardToPos(clampedHover)); }
    int32_t posToBoard(Vector2i p_pos);
    Vector2i boardToPos(int32_t brdPos);
    Vector2i getClampedPiecePos(Vector2i mousePos);

    void toggleView() { activeView = !activeView; }

    void setLegalMoves(std::unordered_set<int32_t> p_legalMoves) { legalMovePositions = p_legalMoves; }
    void clearLegalMoves() { legalMovePositions.clear(); }

    void updateHome(int32_t p_brdPos) { highlightPositions[0] = (p_brdPos != highlightPositions[1] && p_brdPos != highlightPositions[2]) ? p_brdPos : -1; }
    void clearHome() { highlightPositions[0] = -1; }
    void updatePreviousHighlights(int32_t p_from, int32_t p_to);

    int getSquareSize() { return squareSize; }


private:
    RenderWindow &window;

    int32_t clampedHover = 0;

    uint32_t legalMoveColor = 0x23fa8780;
    uint32_t selectedColor = 0xe6db1280;
    uint32_t highlightColor = 0x5c96f280;
    int squareSize = 0;

    Entity board;
    Vector2i boardStart, boardEnd;  // board end is top left corner of bottom right square (position for drawing a piece in that square)

    //std::optional<Vector2i> legalMoves[27];  // 27 is max moves a single piece can have
    //std::optional<Vector2i> highlights[3];
    // would require more work to update if window resized
    std::unordered_set<int32_t> legalMovePositions;
    int32_t highlightPositions[3] = { -1, -1, -1 };
    bool activeView = true;
};