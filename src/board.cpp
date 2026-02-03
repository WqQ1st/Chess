#include "board.h"
#include "attacks.h"
#include <math.h>
#include <iostream>

using std::uint8_t;
using std::uint64_t;


void ChessBoard::print_bitboard(uint64_t bitboard) {
    for (int rank = 0; rank < 8; ++rank) {        // rank 8 → 1
        for (int file = 0; file < 8; ++file) {    // file A → H
            int square = file + rank * 8;
            std::cout << ((bitboard >> square) & 1ULL) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

/*
bool ChessBoard::isWhite(uint64_t square) {
    return getPiece(square) < 6;
}

bool ChessBoard::isBlack(uint64_t square) {
    return (getPiece(square) >= 6 &&  getPiece(square) < 12);
}

bool ChessBoard::validPiece(uint64_t square) {
    if (stateStack[stackIndex].turn == WHITE) {
        return isWhite(square);
    } else {
        return isBlack(square);
    }
}
*/

uint8_t ChessBoard::getPiece(uint8_t square) {
    uint64_t mask = uint64_t(1) << square;
    for (int i = 0; i < 12; ++i) {
        if (stateStack[stackIndex].bitboards[i] & mask) {
            return i;
        }
    }
    return EMPTY;
}

void ChessBoard::setPiece(uint8_t piece, uint8_t square) {
    uint64_t mask = uint64_t(1) << square;
    for (int i = 0; i < 12; ++i) {
        stateStack[stackIndex].bitboards[i] &= ~mask;
    }
    stateStack[stackIndex].bitboards[piece] |= mask;
}

void ChessBoard::move(const Move& move) {
    //push a new state onto the stack
    stackIndex++;
    stateStack[stackIndex] = stateStack[stackIndex - 1];


    uint64_t fromBoard = uint64_t(1) << move.from;
    uint64_t toBoard = uint64_t(1) << move.to;
    uint64_t moveBoard = fromBoard | toBoard;

    //move a piece
    uint8_t movedPiece = EMPTY;
    for (int i = stateStack[stackIndex].turn == WHITE ? 0 : 6; i < (stateStack[stackIndex].turn == WHITE ? 6 : 12); ++i) {
        if (stateStack[stackIndex].bitboards[i] & fromBoard) {
            stateStack[stackIndex].bitboards[i] ^= moveBoard;
            movedPiece = i;
            break;
        }
    }

    //capture a piece
    for (int i = stateStack[stackIndex].turn == WHITE ? 6 : 0; i < (stateStack[stackIndex].turn == WHITE ? 11 : 5); ++i) {
        if (stateStack[stackIndex].bitboards[i] & toBoard) {
            stateStack[stackIndex].bitboards[i] ^= toBoard;
            break;
        }
    }

    //en-passant
    if (toBoard == stateStack[stackIndex].passantTarget) {
        if (movedPiece == WHITE_PAWN) {
            stateStack[stackIndex].bitboards[BLACK_PAWN] ^= stateStack[stackIndex].passantTarget << 8;
        } else if (movedPiece == BLACK_PAWN) {
            stateStack[stackIndex].bitboards[WHITE_PAWN] ^= stateStack[stackIndex].passantTarget >> 8;
        }
    }

    //set en-passant target
    if ((movedPiece == WHITE_PAWN || movedPiece == BLACK_PAWN) && abs(move.from - move.to) == 16) {
        stateStack[stackIndex].passantTarget = uint64_t(1) << int((move.from + move.to) * 0.5);
    } else {
        stateStack[stackIndex].passantTarget = 0;
    }

    //castling
    if (movedPiece == WHITE_KING) {
        if (moveBoard == 0x5000000000000000) {
            stateStack[stackIndex].bitboards[WHITE_ROOK] ^= 0xa000000000000000;
        } else if (moveBoard == 0x1400000000000000) {
            stateStack[stackIndex].bitboards[WHITE_ROOK] ^= 0x0900000000000000;
        }
    }
    else if (movedPiece == BLACK_KING) {
        if (moveBoard == 0x0000000000000050) {
            stateStack[stackIndex].bitboards[BLACK_ROOK] ^= 0x00000000000000a0;
        } else if (moveBoard == 0x0000000000000014) {
            stateStack[stackIndex].bitboards[BLACK_ROOK] ^= 0x0000000000000009;
        }
    }

    //update castling rights
    if (moveBoard & 0x9000000000000000) stateStack[stackIndex].WKC = false;
	if (moveBoard & 0x1100000000000000) stateStack[stackIndex].WQC = false;
	if (moveBoard & 0x0000000000000090) stateStack[stackIndex].BKC = false;
	if (moveBoard & 0x0000000000000011) stateStack[stackIndex].BQC = false;

    //promotions
    if (move.promotion != EMPTY) {
        stateStack[stackIndex].bitboards[stateStack[stackIndex].turn == WHITE ? WHITE_PAWN : BLACK_PAWN] ^= toBoard;
        stateStack[stackIndex].bitboards[move.promotion] ^= toBoard;
    }

    //switch turns
    if (stateStack[stackIndex].turn == WHITE) {
        stateStack[stackIndex].turn = BLACK;
    } else {
        stateStack[stackIndex].turn = WHITE;
    }
}

void ChessBoard::undo() {
    if (stackIndex > 0) {
        --stackIndex;
    }
}

ChessBoard::ChessBoard() {
    stateStack = new BoardState[1000];
    stackIndex = 0;

    //white pieces
    stateStack[0].bitboards[WHITE_PAWN]   = 0x00ff000000000000;
	stateStack[0].bitboards[WHITE_KNIGHT] = 0x4200000000000000;
	stateStack[0].bitboards[WHITE_BISHOP] = 0x2400000000000000;
	stateStack[0].bitboards[WHITE_ROOK]   = 0x8100000000000000;
	stateStack[0].bitboards[WHITE_QUEEN]  = 0x0800000000000000;
	stateStack[0].bitboards[WHITE_KING]   = 0x1000000000000000;

    //black pieces
	stateStack[0].bitboards[BLACK_PAWN]   = 0x000000000000ff00;
	stateStack[0].bitboards[BLACK_KNIGHT] = 0x0000000000000042;
	stateStack[0].bitboards[BLACK_BISHOP] = 0x0000000000000024;
	stateStack[0].bitboards[BLACK_ROOK]   = 0x0000000000000081;
	stateStack[0].bitboards[BLACK_QUEEN]  = 0x0000000000000008;
	stateStack[0].bitboards[BLACK_KING]   = 0x0000000000000010;

    stateStack[0].WKC = true;
    stateStack[0].WQC = true;
    stateStack[0].BKC = true;
    stateStack[0].BQC = true;

    stateStack[0].passantTarget = 0;

    stateStack[0].turn = WHITE;
}

ChessBoard::~ChessBoard() {
    delete[] stateStack;
}
