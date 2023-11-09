#pragma once

#include <SDL.h>
#include <SDL_image.h>

#include <unordered_map>
#include <unordered_set>
#include <stdint.h>
#include <string>
#include <vector>
#include <sstream>
#include <optional>

#include "RenderWindow.hpp"
#include "Board.hpp"
#include "Piece.hpp"
#include "Math.hpp"

struct GameState
{
    bool activeMove = true;
    uint8_t castleOpportunities = 0b1111;
    int32_t enPassant = 0;
    uint32_t halfMoves = 0, fullMoves = 1;
};

class Pieces
{
public:
    Pieces(Board &p_board);
    void setBoard(std::string p_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    void onClick(int32_t p_brdpos);
    void onRelease(int32_t p_brdpos);
    void draw();
    void updateSelectedPiecePos(Vector2i p_mousePos);

    void updateAllPiecePos();

    bool checkMate = false;

private:
    bool selectPieceAtSquare(int32_t p_brdPos);
    bool placeSelectedPiece(int32_t p_brdPos);
    void makeMove(int32_t p_brdPos);

    bool pieceAt(int32_t p_brdPos);
    void movePiece(int32_t from, int32_t end);
    void removePiece(int32_t p_brdPos);

    // calculations
    void calculateAllLegalMoves(bool p_color);
    bool doesMoveResultInCheck(int32_t p_brdPosFrom, int32_t p_brdPosTo, bool p_color);
    bool isThreatened(int32_t p_brdPos, bool p_color);

private:
    GameState state;
    std::unordered_map<int32_t, Piece> storedPieces;                           // 0 is top left
    std::unordered_map<int32_t, std::unordered_set<int32_t>> storedLegalMoves; // why does this work, but not when it is part of Piece struct
    std::optional<PieceAndBoard> selected;
    bool held = false;
    bool unselectIfDropped = false;
    bool inCheck = false;
    int32_t kingPositions[2] = {4, 60};

    RenderWindow &window;
    Board &board;

    const int32_t directions[8] = {-8, 8, -1, 1, -9, 9, 7, -7}; // i % 2 == 1 is movements to right
    const int32_t kightMoves[8] = {6, -10, 15, -17, 17, -15, 10, -6};

    bool flipBoard = false; // determines if the board is flipped after every turn
};
