#pragma once

#include <stdint.h>
#include <unordered_set>
#include <vector>

#include "Math.hpp"

enum PieceTypes : uint32_t
{
    None = 0,
    Pawn = 1,
    Knight = 2,
    Bishop = 3,
    Rook = 4,
    Queen = 5,
    King = 6,
    White = 8,
    Black = 0
};

struct Piece
{
    uint32_t type = 0;
    Vector2i pos = {0, 0};

    Piece(uint32_t p_type, Vector2i p_pos) : type(p_type), pos(p_pos) {}
    Piece() {}
};

struct PieceAndBoard
{
    int32_t brdPos = 0;
    Piece piece;

    PieceAndBoard(int32_t p_brdPos, const Piece &p_piece) : brdPos(p_brdPos), piece(p_piece) {}
};

static std::unordered_map<char, uint32_t> pieceCharToVals{
    {'p', Black | Pawn},
    {'n', Black | Knight},
    {'b', Black | Bishop},
    {'r', Black | Rook},
    {'q', Black | Queen},
    {'k', Black | King},

    {'P', White | Pawn},
    {'N', White | Knight},
    {'B', White | Bishop},
    {'R', White | Rook},
    {'Q', White | Queen},
    {'K', White | King}};