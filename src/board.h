#ifndef BOARD_H
#define BOARD_H

// Bitboard convention: left to right, top to bottom. Right ops >> : right, down. Left ops << : left, up.
// bit 0 = A8
// bit 7 = H8
// bit 56 = A1
// bit 63 = H1

// FEN dedug positions
#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1"
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "

#include <cstdint>
#include "square.h"

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

extern int char_pieces[];

enum Color {
    WHITE = 0,
    BLACK,
    BOTH = 2
};

/*
    bin  dec
    
   0001    1  white king can castle to the king side
   0010    2  white king can castle to the queen side
   0100    4  black king can castle to the king side
   1000    8  black king can castle to the queen side

   examples

   1111       both sides an castle both directions
   1001       black king => queen side
              white king => king side

*/

enum { wk = 1, wq = 2, bk = 4, bq = 8 };


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
    uint64_t bitboards[12]{};

    //occupancies for white, black, all
    uint64_t occupancies[3]{};

    //castling rights
    uint8_t castle = wk | wq | bk | bq; //all castling rights preserved, 1111

    //bitboard that represents en-passant target
    uint64_t passantTarget = 0;

    //whose turn it is
    uint8_t turn = WHITE;

    //update occupancies to reflect current board
    void update_occupancies() {
        occupancies[WHITE] = 0ULL;
        occupancies[BLACK] = 0ULL;

        for (int i = 0; i < 6; ++i) {
            occupancies[WHITE] |= bitboards[i];
        }

        for (int i = 6; i < 12; ++i) {
            occupancies[BLACK] |= bitboards[i];
        }

        occupancies[2] = occupancies[WHITE] | occupancies[BLACK];
    }

    int piece_on(uint8_t square) {
        int piece = EMPTY;
        for (int i = 0; i < 12; ++i) {
            if (bitboards[i] && square) {
                piece = i;
                break;
            }
        }
        return piece;
    }
};

class ChessBoard {
    private:
        //stack of board states
        BoardState* stateStack;
        int stackIndex;

    public:
        void print_bitboard(uint64_t bitboard);

        BoardState parse_fen(const char *fen);

        //setters and getters on a specific square
        uint8_t getPiece(uint8_t square);
        void setPiece(uint8_t piece, uint8_t square);

        //move and undo
        void move(const Move& move);
        void undo();

    //constructor
    ChessBoard();
    ChessBoard(const char *fen);

    //destructor
    ~ChessBoard();
};

#endif
