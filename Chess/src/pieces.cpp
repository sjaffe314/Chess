#include "Pieces.hpp"

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <initializer_list>
#include <cmath>

Pieces::Pieces(RenderWindow &p_window, Board &p_board)
    : window(p_window), board(p_board)
{
    setBoard();

    calculateAllLegalMoves(state.activeMove);
}

void Pieces::setBoard(std::string p_fen)
{
    int32_t brdPos = 0;

    int i = 0;
    for (char c = p_fen[i]; c != ' '; c = p_fen[++i])
    {
        if (isdigit(c))
        {
            brdPos += int(c - '0');
            continue;
        }
        else if (isalpha(c))
        {
            storedPieces[brdPos] = Piece(pieceCharToVals[c], board.boardToPos(brdPos));
            if (c == 'k')
                kingPositions[0] = brdPos;
            else if (c == 'K')
                kingPositions[1] = brdPos;
            brdPos += 1;
        }
    }

    state.activeMove = p_fen[++i] == 'w';

    std::unordered_map<char, uint8_t> translate = {{'q', 0b1}, {'k', 0b10}, {'Q', 0b100}, {'K', 0b1000}};
    for (char c = p_fen[i += 2]; c != ' '; c = p_fen[++i])
    {
        state.castleOpportunities |= translate[c];
    }

    if (p_fen[++i] != '-')
    {
        state.enPassant = (p_fen[i] - 'a') + (p_fen[i + 1] - '1') * 8;
        i++;
    }

    state.halfMoves = p_fen[i += 2] - '0';
    if (p_fen[++i] != ' ')
    {
        state.halfMoves = state.halfMoves * 10 + p_fen[i++] - '0'; // the ++ moves to the next space
    }

    state.fullMoves = p_fen[++i] - '0';
    for (i++; i < (int)p_fen.size(); i++)
    {
        state.fullMoves = state.fullMoves * 10 + p_fen[i] - '0';
    }
}

void Pieces::onClick(int32_t p_brdPos)
{
    if (selected.has_value())
    {
        if (p_brdPos == selected->brdPos)
        {
            held = true;
            unselectIfDropped = true;
            return;
        }

        if (placeSelectedPiece(p_brdPos)) // attempts to place piece, returns if successful
            return;

        selected.reset();
        board.clearHome();
        board.clearLegalMoves();
    }
    selectPieceAtSquare(p_brdPos);
}

void Pieces::onRelease(int32_t p_brdPos)
{
    if (held)
        placeSelectedPiece(p_brdPos);
}

void Pieces::draw()
{
    if (held)
    {
        board.drawHover();
        for (auto &[brdPos, piece] : storedPieces)
        {
            if (brdPos == selected->brdPos)
                continue;
            window.renderPiece(piece.type, piece.pos);
        }
        window.renderPiece(selected->piece.type, selected->piece.pos);
        return;
    }
    for (auto &[brdPos, piece] : storedPieces)
    {
        window.renderPiece(piece.type, piece.pos);
    }
}

void Pieces::updateSelectedPiecePos(Vector2i p_mousePos)
{
    if (held)
        selected->piece.pos = board.getClampedPiecePos(p_mousePos);
}

void Pieces::updateAllPiecePos()
{
    for (auto& [brdPos, piece] : storedPieces)
    {
        piece.pos = board.boardToPos(brdPos);
    }
}

bool Pieces::selectPieceAtSquare(int32_t p_brdPos)
{
    /*Selects piece at square. Returns whether it was sucessful*/
    if (!storedPieces.contains(p_brdPos))
        return false;

    selected = {p_brdPos, storedPieces.at(p_brdPos)};
    board.updateHome(p_brdPos); // highlight square of piece taken
    board.setLegalMoves(storedLegalMoves[p_brdPos]);

    held = true;
    unselectIfDropped = false;
    return true;
}

bool Pieces::placeSelectedPiece(int32_t p_brdPos)
{
    held = false;
    if (storedLegalMoves[selected->brdPos].contains(p_brdPos)) // if move is legal
    {
        changeTurn(p_brdPos);
        return true;
    }

    selected->piece.pos = board.boardToPos(p_brdPos);

    if (p_brdPos == selected->brdPos && unselectIfDropped)
    {
        selected.reset();
        board.clearHome();
        board.clearLegalMoves();
        return true;
    }
    // illegal move or drop in same place, resets piece location, keeps selected though
    return false;
}

void Pieces::movePiece(int32_t from, int32_t end)
{
    storedPieces[end] = storedPieces[from];
    storedPieces[end].pos = board.boardToPos(end);
    storedPieces.erase(from);
}

void Pieces::removePiece(int32_t p_brdPos)
{
    storedPieces.erase(p_brdPos);
}

void Pieces::changeTurn(int32_t p_brdPos)
{
    board.updatePreviousHighlights(selected->brdPos, p_brdPos);
    board.clearLegalMoves();
    board.toggleView();
    updateAllPiecePos();

    // updates board data & deals with special cases (promotions/castles/enPassants)
    bool storeEnPassant = false;
    switch (selected->piece.type & 0b111)
    {
    case King:
        if (abs(p_brdPos - selected->brdPos) == 2) // means it is a castle so have to move rook
        {
            int rookPos = p_brdPos + (p_brdPos - selected->brdPos + 2) / 4 * 3 - 2;
            int newRookPos = (p_brdPos + selected->brdPos) / 2;
            Pieces::movePiece(rookPos, newRookPos);
        }
        kingPositions[int(state.activeMove)] = p_brdPos;
        if (selected->brdPos == 4)
            state.castleOpportunities &= 0b1100;
        else if (selected->brdPos == 60)
            state.castleOpportunities &= 0b0011;
        break;
    case Rook:
        if (selected->brdPos == 0)
            state.castleOpportunities &= 0b1110;
        else if (selected->brdPos == 7)
            state.castleOpportunities &= 0b1101;
        else if (selected->brdPos == 56)
            state.castleOpportunities &= 0b1011;
        else if (selected->brdPos == 63)
            state.castleOpportunities &= 0b0111;
        break;
    case Pawn:
        if (p_brdPos / 8 == 7 || p_brdPos / 8 == 0)
            selected->piece.type = (int(state.activeMove) << 3) | Queen; // modify here if want to give option for which piece
        else if (p_brdPos == state.enPassant)
            Pieces::removePiece(p_brdPos + (int)state.activeMove * 16 - 8);
        storeEnPassant = abs(selected->brdPos - p_brdPos) == 16; // if it moved 2 rows
    default:
        break;
    }
    state.enPassant = storeEnPassant ? (selected->brdPos + p_brdPos) / 2 : 0;
    state.activeMove = !state.activeMove;
    state.fullMoves++;

    Pieces::movePiece(selected->brdPos, p_brdPos);
    selected.reset();

    calculateAllLegalMoves(state.activeMove);
}

void Pieces::calculateAllLegalMoves(bool p_color)
{
    storedLegalMoves.clear();
    std::unordered_set<int32_t> legalMoves;

    uint32_t enemyColor = int(!p_color) << 3;
    int32_t kingPos = kingPositions[int(p_color)];

    bool inCheck = false;
    bool doubleCheck = false;

    // iteration variables
    int i;
    int32_t direction;
    int32_t option;

    // check 8 directions for threats
    uint32_t pieceType;
    for (direction = directions[i = 0]; i < 8; direction = directions[++i])
    {
        option = kingPos + direction;
        if (!(option < 64 && option >= 0 && (i < 2 || !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7)))))
            continue;

        if (storedPieces.contains(option)) // needs to have contrait of 0-63 if changed to array
        {
            if (bool(storedPieces[option].type >> 3) != p_color)
            {
                legalMoves.insert(option); // only not an option if same color (It goes back to double check that it isn't a protected piece later)
                pieceType = storedPieces[option].type & 0b111;
                if (pieceType == Queen || pieceType == (i < 4 ? Rook : Bishop) || (pieceType == Pawn && (i == int(!p_color) + 4 || i == int(p_color) + 6))) // i is refering to the index of the direction (white: 4 & 7, black: 5 & 6)
                {
                    if (pieceType != Pawn)
                    {
                        legalMoves.erase(kingPos - direction);
                        i = ((i / 2) + 1) * 2; // moves i to next direction
                    }
                    if (inCheck)
                        doubleCheck = true;
                    else
                        inCheck = true;
                }
            }
            continue;
        }

        legalMoves.insert(option);
        // previous section necessary for king because the king can only move one place

        for (option += direction; option < 64 && option >= 0 && (i < 2 || !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7))); option += direction)
        {
            if (storedPieces.contains(option))
            {
                if (bool(storedPieces[option].type >> 3) != p_color)
                {
                    pieceType = storedPieces[option].type & 0b111;
                    if (pieceType == Queen || pieceType == (i < 4 ? Rook : Bishop))
                    {
                        legalMoves.erase(kingPos + direction);
                        legalMoves.erase(kingPos - direction);
                        i = ((i / 2) + 1) * 2; // moves i to next direction
                        if (inCheck)
                            doubleCheck = true;
                        else
                            inCheck = true;
                    }
                }
                break;
            }
        }
    }

    // check horse positions for threats
    for (option = kingPos + kightMoves[i = 0]; i < 8; option = kingPos + kightMoves[++i])
    {
        if (storedPieces.contains(option) && storedPieces[option].type == (enemyColor | Knight))
        {
            if (inCheck)
                doubleCheck = true;
            else
                inCheck = true;
        }
    }

    // place holder until can be optimized (this double checks many directions)
    for (auto it = legalMoves.begin(); it != legalMoves.end();)
    {
        if (doesMoveResultInCheck(kingPos, *it, p_color))
            it = legalMoves.erase(it);
        else
            ++it;
    }

    if (!inCheck)
    {
        for (direction = directions[i = 2]; i < 4; direction = directions[++i])
        {
            if (state.castleOpportunities & (0b1 << ((int)p_color << 1) << (i - 2))                                   // First checks the 0b01 (queen side) then checks the 0b10 (king side)
                && legalMoves.contains(kingPos + direction)                                                           // move is already checked for being empty and not threatened
                && !storedPieces.contains(kingPos + 2 * direction) && !isThreatened(kingPos + 2 * direction, p_color) // checks if final kingPos is blocked or threated
                && (direction == 1 || !storedPieces.contains(kingPos - 3)))                                           // checks final square if it is queen side
            {
                legalMoves.insert(kingPos + 2 * direction);
            }
        }
    }

    if (!legalMoves.empty())
        storedLegalMoves[kingPos] = legalMoves;

    if (!doubleCheck) // king must move if in double check
    {
        for (int brdPos = 0; brdPos < 64; brdPos++)
        {
            //  || pinnedPieces.contains(brdPos)
            // can't use because can move in line with the pin... maybe way to solve that
            if (!storedPieces.contains(brdPos) || bool(storedPieces[brdPos].type >> 3) != p_color)
                continue;

            legalMoves.clear();

            switch (storedPieces[brdPos].type & 0b111) // masks for just type
            {
            case Pawn:
            {
                int32_t dir = 2 * int32_t(!p_color) - 1;
                for (direction = dir * 7; direction != dir * 11; direction += 2 * dir)
                {
                    option = brdPos + direction;
                    if (((storedPieces.contains(option) && bool(storedPieces[option].type >> 3) == !p_color) || option == state.enPassant) && !doesMoveResultInCheck(brdPos, option, p_color))
                        legalMoves.insert(option);
                }

                direction = dir * 8;
                option = brdPos + direction;
                if (!storedPieces.contains(option))
                {
                    if (!doesMoveResultInCheck(brdPos, option, p_color))
                        legalMoves.insert(option);
                    option += direction;
                    if (brdPos / 8 == 1 + 5 * int(p_color) && !storedPieces.contains(option) && !doesMoveResultInCheck(brdPos, option, p_color))
                        legalMoves.insert(option);
                }
                break;
            }
            case Knight:
            {
                int end = brdPos & 7; // brdPos % 8
                int i = (2 - __min(end, 2)) * 2;
                end = (5 - __max(end, 5)) * 2 + 8;

                for (option = brdPos + kightMoves[i]; i < end; option = brdPos + kightMoves[++i])
                {
                    if (option < 64 && option >= 0 && (!storedPieces.contains(option) || bool(storedPieces[option].type >> 3) == !p_color) && !doesMoveResultInCheck(brdPos, option, p_color))
                        legalMoves.insert(option);
                }
                break;
            }
            case Bishop:
            {
                int i = 4;
                for (direction = directions[i]; i < 8; direction = directions[++i])
                {
                    for (option = brdPos + direction; option < 64 && option >= 0 && !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7)); option += direction)
                    {
                        if (doesMoveResultInCheck(brdPos, option, p_color))
                            continue;
                        if (storedPieces.contains(option))
                        {
                            if (bool(storedPieces[option].type >> 3) != p_color)
                                legalMoves.insert(option);
                            break;
                        }
                        legalMoves.insert(option);
                    }
                }
                break;
            }
            case Rook:
            {
                int i = 0;
                for (direction = directions[i]; i < 4; direction = directions[++i])
                {
                    for (option = brdPos + direction; option < 64 && option >= 0 && (i < 2 || !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7))); option += direction)
                    {
                        if (doesMoveResultInCheck(brdPos, option, p_color))
                            continue;
                        if (storedPieces.contains(option))
                        {
                            if (bool(storedPieces[option].type >> 3) != p_color)
                                legalMoves.insert(option);
                            break;
                        }
                        legalMoves.insert(option);
                    }
                }
                break;
            }
            case Queen:
            {
                int i = 0;
                for (direction = directions[i]; i < 8; direction = directions[++i])
                {
                    for (option = brdPos + direction; option < 64 && option >= 0 && (i < 2 || !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7))); option += direction)
                    {
                        if (doesMoveResultInCheck(brdPos, option, p_color))
                            continue;
                        if (storedPieces.contains(option))
                        {
                            if (bool(storedPieces[option].type >> 3) != p_color)
                                legalMoves.insert(option);
                            break;
                        }
                        legalMoves.insert(option);
                    }
                }
                break;
            }
            }

            if (!legalMoves.empty())
                storedLegalMoves[brdPos] = legalMoves;
        }
    }

    if (storedLegalMoves.empty())
    {
        if (inCheck)
            checkMate = true;
        // else
        //     stalemate
    }
}

bool Pieces::isThreatened(int32_t p_brdPos, bool p_color)
{
    if (p_brdPos >= 64)
        return false;

    // bool color = bool(storedPieces[p_brdPos].type >> 3);
    uint32_t enemyColor = int(!p_color) << 3;

    int32_t option;
    uint32_t pieceType;

    int i;
    for (int32_t direction = directions[i = 0]; i < 8; direction = directions[++i])
    {
        option = p_brdPos + direction;
        if (!(option < 64 && option >= 0 && (i < 2 || !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7)))))
            continue;
        if (storedPieces.contains(option)) // needs to have contrait of 0-63 if changed to array
        {
            if (bool(storedPieces[option].type >> 3) != p_color)
            {
                pieceType = storedPieces[option].type & 0b111;
                if (pieceType == (i < 4 ? Rook : Bishop) || pieceType == Queen || pieceType == King || ((i == int(!p_color) + 4 || i == int(p_color) + 6) && pieceType == Pawn))
                    return true;
            }
            continue;
        }
        // previous section necessary for king because the king and pawn can only move one place

        for (option += direction; option < 64 && option >= 0 && (i < 2 || !((i % 2 == 1 && option % 8 == 0) || (i % 2 == 0 && option % 8 == 7))); option += direction)
        {
            if (storedPieces.contains(option))
            {
                if (bool(storedPieces[option].type >> 3) != p_color)
                {
                    pieceType = storedPieces[option].type & 0b111;
                    if (pieceType == (i < 4 ? Rook : Bishop) || pieceType == Queen)
                        return true;
                }
                break;
            }
        }
    }

    for (option = p_brdPos + kightMoves[i = 0]; i < 8; option = p_brdPos + kightMoves[++i])
    {
        if (storedPieces.contains(option) && storedPieces[option].type == (enemyColor | Knight))
            return true;
    }

    return false;
}

bool Pieces::doesMoveResultInCheck(int32_t p_brdPosFrom, int32_t p_brdPosTo, bool p_color)
{
    bool inCheck = false;
    bool isKing = (storedPieces[p_brdPosFrom].type & 0b111) == King;

    Piece backUp;
    bool isCapture = storedPieces.contains(p_brdPosTo);
    if (isCapture)
        backUp = storedPieces[p_brdPosTo];

    storedPieces[p_brdPosTo] = storedPieces[p_brdPosFrom];
    storedPieces.erase(p_brdPosFrom); // doesn't seem to, but could interfer with selected piece pointer

    if (isThreatened((isKing) ? p_brdPosTo : kingPositions[int(p_color)], p_color))
        inCheck = true;

    storedPieces[p_brdPosFrom] = storedPieces[p_brdPosTo];
    if (isCapture)
        storedPieces[p_brdPosTo] = backUp;
    else
        storedPieces.erase(p_brdPosTo);
    return inCheck;
}

/*
bool Pieces::tryMove(int32_t p_brdPos)
{
    if (p_brdPos >= 64)
        return false;

    bool color = bool(selected->piece.type >> 3);

    if (color != state.activeMove)
        return false;

    bool isCapture = false;
    if (storedPieces.contains(p_brdPos))
    {
        if (bool(storedPieces[p_brdPos].type >> 3) != color)
            isCapture = true;
        else
            return false;
    }

    bool keepEnpassant = false;
    bool promote = false;
    int32_t diff = p_brdPos - selected->brdPos;

    switch (selected->piece.type & 0b111) // masks for just type
    {
    case Pawn:
        if (color)
        {
            promote = p_brdPos / 8 == 0;
            if (isCapture)
            {
                if (diff == -9 || diff == -7)
                    break;
            }
            else
            {
                if (diff == -8)
                    break;
                if (diff == -16 && selected->brdPos / 8 == 6 && !storedPieces.contains(p_brdPos + 8))
                {
                    state.enPassant = p_brdPos + 8;
                    keepEnpassant = true;
                    break;
                }
                if (state.enPassant == p_brdPos && (diff == -9 || diff == -7))
                {
                    storedPieces.erase(state.enPassant + 8);
                    break;
                }
            }
        }
        else
        {
            promote = p_brdPos / 8 == 7;
            if (isCapture)
            {
                if (diff == 9 || diff == 7)
                    break;
            }
            else
            {
                if (diff == 8)
                    break;
                if (diff == 16 && selected->brdPos / 8 == 1 && !storedPieces.contains(p_brdPos - 8))
                {
                    state.enPassant = p_brdPos - 8;
                    keepEnpassant = true;
                    break;
                }
                if (state.enPassant == p_brdPos && (diff == 9 || diff == 7))
                {
                    storedPieces.erase(state.enPassant - 8);
                    break;
                }
            }
        }
        return false;
    case Knight:
    {
        int end = p_brdPos & 7; // brdPos % 8
        int i = (2 - __min(end, 2)) * 2;
        end = (5 - __max(end, 5)) * 2 + 8;

        for (int diffCanidate = kightMoves[i]; i < end; diffCanidate = kightMoves[++i])
        {
            if (diff == diffCanidate)
                break;
        }
        return false;
        // if (is_in(diff, {6, 10, 15, 17, -6, -10, -15, -17}))
        //     break;
        // return false;
        // ^^^ didn't have guards against wrapping around the board
    }
    case Bishop:
    {
        int increment;
        if (!(diff % 7))
            increment = 7 * abs(diff) / diff;
        else if (!(diff % 9))
            increment = 9 * abs(diff) / diff;
        else
            return false;
        for (int i = selected->brdPos + increment; i != p_brdPos; i += increment)
        {
            if (storedPieces.contains(i))
                return false;
        }
        break;
    }
    case Rook:
    {
        int increment;
        if (!(diff % 8))
            increment = 8 * abs(diff) / diff;
        else if (selected->brdPos / 8 == p_brdPos / 8)
            increment = abs(diff) / diff; // 1 * is implied because step is only 1
        else
            return false;
        for (int i = selected->brdPos + increment; i != p_brdPos; i += increment)
        {
            if (storedPieces.contains(i))
                return false;
        }
        break;
    }
    case Queen:
    {
        int increment;
        if (!(diff % 7))
            increment = 7 * abs(diff) / diff;
        else if (!(diff % 9))
            increment = 9 * abs(diff) / diff;
        else if (!(diff % 8))
            increment = 8 * abs(diff) / diff;
        else if (selected->brdPos / 8 == p_brdPos / 8)
            increment = abs(diff) / diff; // 1 * is implied because step is only 1
        else
            return false;
        for (int i = selected->brdPos + increment; i != p_brdPos; i += increment)
        {
            if (storedPieces.contains(i))
                return false;
        }
        break;
    }
    case King:
    {
        // {{'q', 0b1}, {'k', 0b10}, {'Q', 0b100}, {'K', 0b1000}}
        if (kingInfos[int(color)].moves[diff])
            break;
        else if (!isThreatened(selected->brdPos, color))
        {
            int32_t rookPos;
            if (color)
            {
                if (p_brdPos == 58 && state.castleOpportunities & 0b0001)
                    rookPos = 56;
                else if (p_brdPos == 62 && state.castleOpportunities & 0b0010)
                    rookPos = 63;
                else
                    return false;
            }
            else
            {
                if (p_brdPos == 2 && state.castleOpportunities & 0b0100)
                    rookPos = 0;
                else if (p_brdPos == 6 && state.castleOpportunities & 0b1000)
                    rookPos = 7;
                else
                    return false;
            }
            int dir = int(rookPos > selected->brdPos) * 2 - 1;
            int j = 0;
            for (int i = selected->brdPos + dir; i != rookPos; i += dir)
            {
                if (storedPieces.contains(i) || (j < 2 && isThreatened(i, state.activeMove)))
                    return false;
                j++;
            }
            storedPieces[rookPos].pos = window.boardToPos(selected->brdPos + dir);
            storedPieces[selected->brdPos + dir] = storedPieces[rookPos];
            storedPieces.erase(rookPos);
            break;
        }
        return false;
    }
        // case default:
        //     return false;
    }
    Piece backUp;
    if (isCapture)
        backUp = storedPieces[p_brdPos];

    storedPieces[p_brdPos] = storedPieces[selected->brdPos];
    storedPieces.erase(selected->brdPos);

    if (isThreatened((selected->piece.type & 0b111) == King ? p_brdPos : kingPositions[int(color)], color))
    {
        storedPieces[selected->brdPos] = storedPieces[p_brdPos];
        if (isCapture)
            storedPieces[p_brdPos] = backUp;
        else
            storedPieces.erase(p_brdPos);
        return false;
    }
    if (promote)
        // selected->piece.type = ((int(color) << 3) | Queen); // modify here if want to give option for which piece
        storedPieces[p_brdPos].type = ((int(color) << 3) | Queen); // modify here if want to give option for which piece
    if (!keepEnpassant)
        state.enPassant = 0;

    switch (selected->piece.type & 0b111)
    {
    case King:
        kingPositions[int(color)] = p_brdPos;
        // ends castle opportunities
        if (selected->brdPos == 4)
            state.castleOpportunities &= 0b0011;
        if (selected->brdPos == 60)
            state.castleOpportunities &= 0b1100;
        break;
    case Rook:
        // break castleOpportunities
        if (selected->brdPos == 0)
            state.castleOpportunities &= 0b1110;
        if (selected->brdPos == 7)
            state.castleOpportunities &= 0b1101;
        if (selected->brdPos == 56)
            state.castleOpportunities &= 0b1011;
        if (selected->brdPos == 63)
            state.castleOpportunities &= 0b0111;
    default:
        break;
    }

    storedPieces[p_brdPos].pos = window.boardToPos(p_brdPos);
    selected.reset();
    return true;
}
*/
