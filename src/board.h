#ifndef BOARD_H
#define BOARD_H

// Bitboard convention: left to right, top to bottom
// bit 0 = A8
// bit 7 = H8
// bit 56 = A1
// bit 63 = H1

#include <cstdint>

enum Piece {
    WHITE_PAWN = 0,
    WHITE_KNIGHT,
    WHITE_BISHOP,
    WHITE_ROOK,
    WHITE_QUEEN,
    WHITE_KING,
    BLACK_PAWN,
    BLACK_KNIGHT,
    BLACK_BISHOP,
    BLACK_ROOK,
    BLACK_QUEEN,
    BLACK_KING,
    EMPTY = 12
};


enum Color {
    WHITE = 0,
    BLACK = 1
};


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
        void print_bitboard(uint64_t bitboard);

        //checking if square has white/black piece on it
        bool isWhite(uint64_t square);
        bool isBlack(uint64_t square);

        bool validPiece(uint64_t square); //might delete these 3

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
