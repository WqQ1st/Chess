#ifndef BOARD_H
#define BOARD_H

#include <cstdint>

#define WHITE_PAWN 0
#define WHITE_KNIGHT 1
#define WHITE_BISHOP 2
#define WHITE_ROOK 3
#define WHITE_QUEEN 4
#define WHITE_KING 5
#define BLACK_PAWN 6
#define BLACK_KNIGHT 7
#define BLACK_BISHOP 8
#define BLACK_ROOK 9
#define BLACK_QUEEN 10
#define BLACK_KING 11
#define EMPTY 12

#define WHITE 13
#define BLACK 14

using std::uint8_t;
using std::uint64_t;

struct Move {
    //move coords
    uint8_t from;
    uint8_t to;

    //promotions
    uint8_t promotion;
};

struct BoardState {
    //Bitboards for each piece type
    uint64_t bitboards[12];

    //castling rights
    bool WKC, BKC, WQC, BQC;

    //bitboard that represents en-passant target
    uint64_t passantTarget;

    //whose turn it is
    uint8_t turn;
};

class ChessBoard {
    private:
        //stack of board states
        BoardState* stateStack;
        int stackIndex;

    public:
        //checking if square has white/black piece on it
        bool isWhite(uint64_t square);
        bool isBlack(uint64_t square);

        bool validPiece(uint64_t square);

        //setters and getters on a specific square
        uint8_t getPiece(uint8_t square);
        void setPiece(uint8_t piece, uint8_t square);

        //move and undo
        void move(const Move& move);
        void undo();

    //constructor
    ChessBoard();

    //destructor
    ~ChessBoard();
};

#endif
