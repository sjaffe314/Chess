#include "Board.hpp"

Board::Board(RenderWindow& p_window) : window(p_window) { updateBoardTexture(); }

void Board::updateBoardTexture()
{
    squareSize = window.getHeight() / 11;
    boardStart = { (window.getWidth() >> 1) - (squareSize << 2), (window.getHeight() >> 1) - (squareSize << 2) };
    boardEnd = boardStart + (squareSize * 7);

    board.updateVals(boardStart - (squareSize / 3), window.createBoardTexture());
}

void Board::draw()
{
    window.render(board);

    for (int i = 0; i < 3; i++)
    {
        if (highlightPositions[i] < 0)
            continue;
        window.renderSquareHighlight(highlightColor, boardToPos(highlightPositions[i]));
    }
    for (const auto& pos : legalMovePositions)
    {
        window.renderSquareHighlight(legalMoveColor, boardToPos(pos));
    }
}

int32_t Board::posToBoard(Vector2i p_pos)
{
    Vector2i Square = ((activeView) ? (p_pos - boardStart) : (boardEnd + squareSize - p_pos)) / squareSize;
    int32_t hover = (Square.x < 0 || Square.x > 7 || Square.y < 0 || Square.y > 7) ? -1 : Square.x + Square.y * 8;
    Square = Square.clamped({ 0, 0 }, { 7, 7 });
    clampedHover = Square.x + Square.y * 8;
    return hover;
}

Vector2i Board::boardToPos(int32_t brdPos)
{
    Vector2i pos = { (brdPos & 0b111) * squareSize, (brdPos >> 3) * squareSize };
    return (activeView) ? boardStart + pos : boardEnd - pos;
}

Vector2i Board::getClampedPiecePos(Vector2i mousePos)
{
    return mousePos.clamped(boardStart, boardEnd + squareSize) - (squareSize / 2);
}

void Board::updatePreviousHighlights(int32_t p_from, int32_t p_to)
{
    highlightPositions[1] = p_from;
    highlightPositions[2] = p_to;
    highlightPositions[0] = -1;
}