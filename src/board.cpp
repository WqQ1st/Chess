#include "board.h"
#include "attacks.h"
#include <math.h>
#include <iostream>
#include "bitboards.h"
#include "square.h"

using std::uint8_t;
using std::uint64_t;

int char_pieces[] = {
    ['P'] = WHITE_PAWN,
    ['N'] = WHITE_KNIGHT,
    ['B'] = WHITE_BISHOP,
    ['R'] = WHITE_ROOK,
    ['Q'] = WHITE_QUEEN,
    ['K'] = WHITE_KING,
    ['p'] = BLACK_PAWN,
    ['n'] = BLACK_KNIGHT,
    ['b'] = BLACK_BISHOP,
    ['r'] = BLACK_ROOK,
    ['q'] = BLACK_QUEEN,
    ['k'] = BLACK_KING,
};

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

void ChessBoard::print_bitboard() {
    for (int i = 0; i < 12; i++) {
        print_bitboard(stateStack[stackIndex].bitboards[i]);
    }
    std::cout << "\n";
}

void ChessBoard::print_occupancy() {
    for (int rank = 0; rank < 8; ++rank) {        // rank 8 → 1
        for (int file = 0; file < 8; ++file) {    // file A → H
            int square = file + rank * 8;
            std::cout << ((stateStack[stackIndex].occupancies[2] >> square) & 1ULL) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

/*
static inline int count_bits(uint64_t bitboard) {
    int count = 0;

    //reset least significant first bit
    while (bitboard) {
        count++;
        bitboard &= bitboard - 1;
    }

    //return bit count
    return count;
}

static inline int get_ls1b_index(uint64_t bitboard) {
    //make sure bitboard not 0
    if (bitboard) {
        return countbits((bitboard & -bitboard) - 1);
    } else {
        return -1; //illegal index
    }
}
*/

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

    //move a piece and store which type (ex: white pawn)
    uint8_t movedPiece = EMPTY;
    for (int i = stateStack[stackIndex].turn == WHITE ? 0 : 6; i < (stateStack[stackIndex].turn == WHITE ? 6 : 12); ++i) {
        if (stateStack[stackIndex].bitboards[i] & fromBoard) {
            stateStack[stackIndex].bitboards[i] ^= moveBoard;
            movedPiece = i;
            break;
        }
    }

    int capturedPiece = EMPTY;

    //capture a piece
    for (int i = stateStack[stackIndex].turn == WHITE ? 6 : 0; i < (stateStack[stackIndex].turn == WHITE ? 11 : 5); ++i) {
        if (stateStack[stackIndex].bitboards[i] & toBoard) {
            stateStack[stackIndex].bitboards[i] ^= toBoard;
            capturedPiece = i;
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
    /*
    if (movedPiece == WHITE_KING) {
        if (moveBoard == 0x5000000000000000) {
            stateStack[stackIndex].bitboards[WHITE_ROOK] ^= 0xa000000000000000; //king side
        } else if (moveBoard == 0x1400000000000000) {
            stateStack[stackIndex].bitboards[WHITE_ROOK] ^= 0x0900000000000000;
        }
    }
    else if (movedPiece == BLACK_KING) {
        if (moveBoard == 0x0000000000000050) {
            stateStack[stackIndex].bitboards[BLACK_ROOK] ^= 0x00000000000000a0; //king side
        } else if (moveBoard == 0x0000000000000014) {
            stateStack[stackIndex].bitboards[BLACK_ROOK] ^= 0x0000000000000009;
        }
    }
    */

    auto& st = stateStack[stackIndex];

    // castle move: king moved two squares, so move rook too
    if (movedPiece == WHITE_KING) {
        if (moveBoard == (BB(E1) | BB(G1)) && (st.castle & wk)) {          // white king side
            st.bitboards[WHITE_ROOK] ^= (BB(H1) | BB(F1));
        } else if (moveBoard == (BB(E1) | BB(C1)) && (st.castle & wq)) {   // white queen side
            st.bitboards[WHITE_ROOK] ^= (BB(A1) | BB(D1));
        }
    }
    else if (movedPiece == BLACK_KING) {
        if (moveBoard == (BB(E8) | BB(G8)) && (st.castle & bk)) {          // black king side
            st.bitboards[BLACK_ROOK] ^= (BB(H8) | BB(F8));
        } else if (moveBoard == (BB(E8) | BB(C8)) && (st.castle & bq)) {   // black queen side
            st.bitboards[BLACK_ROOK] ^= (BB(A8) | BB(D8));
        }
    }


    //update castling rights
    
    //king moved: lose both rights
    if (movedPiece == WHITE_KING) st.castle &= ~(wk | wq);
    if (movedPiece == BLACK_KING) st.castle &= ~(bk | bq);

    //rook moved from its home square: lose that side
    if (movedPiece == WHITE_ROOK) {
        if (fromBoard & BB(H1)) st.castle &= ~wk;
        if (fromBoard & BB(A1)) st.castle &= ~wq;
    }
    else if (movedPiece == BLACK_ROOK) {
        if (fromBoard & BB(H8)) st.castle &= ~bk;
        if (fromBoard & BB(A8)) st.castle &= ~bq;
    }

    //rook captured on its home square: lose that side
    if (capturedPiece == WHITE_ROOK) {
        if (toBoard & BB(H1)) st.castle &= ~wk;
        if (toBoard & BB(A1)) st.castle &= ~wq;
    }
    else if (capturedPiece == BLACK_ROOK) {
        if (toBoard & BB(H8)) st.castle &= ~bk;
        if (toBoard & BB(A8)) st.castle &= ~bq;
    }


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

    stateStack[stackIndex].update_occupancies();
}

void ChessBoard::undo() {
    if (stackIndex > 0) {
        --stackIndex;
    }
}

//parse FEN string
BoardState ChessBoard::parse_fen(const char *fen) {
    //empty board
    BoardState board_state;

    //loop over board
    for (int rank = 0; rank < 8; ++rank) {
        for (int file = 0; file < 8; ++file) {
            int square = rank * 8 + file;

            //match ascii pieces within FEN string
            if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z')) {
                int piece = char_pieces[*fen];

                //set piece on bitboard
                board_state.bitboards[piece] |= (uint64_t(1) << square);

                //increment pointer
                fen++;
            }

            //match empty square numbers
            if (*fen >= '0' && *fen <= '9') {
                file += (*fen - '0');
                fen++;
            }

            //file separator
            if (*fen == '/') {
                fen++;
            }
        }
    }

    //increment pointer to fen string
    while (*fen != ' ') {
        fen++;
    }
    while (*fen == ' ') {
        fen++;
    }

    //parse side to move
    board_state.turn = (*fen == 'w') ? WHITE : BLACK;

    //advance to next field (castling)
    while (*fen != ' ') {
        fen++;
    }
    while (*fen == ' ') {
        fen++;
    }

    //parse castling rights
    board_state.castle = 0; //resets castling rights

    while (*fen != ' ') {
        switch (*fen) {
            case 'K':
                board_state.castle |= wk;
                break;
            case 'Q':
                board_state.castle |= wq;
                break;
            case 'k':
                board_state.castle |= bk;
                break;
            case 'q':
                board_state.castle |= bq;
                break;
            case '-':
                break;
        }

        fen++;
    }

    //go to en passant square
    fen++;

    //parse en passant square
    if (*fen != '-') {
        //parse en passant file and rank
        int file = fen[0] - 'a';
        int rank = 8 - (fen[1] - '0'); //string's 8th rank is 0 in code

        board_state.passantTarget = (uint64_t(1) << (rank * 8 + file));
    }

    board_state.update_occupancies();

    //TODO: add parsing for the last two numbers, turns after moved pawn/piece capture and turns played.

    return board_state;
}

BoardState ChessBoard::curr_state() {
    return stateStack[stackIndex];
}

ChessBoard::ChessBoard(const char *fen) {
    stateStack = new BoardState[1000];
    stackIndex = 0;

    stateStack[0] = parse_fen(fen);
    //parse fen should alr update occupancies
}

ChessBoard::ChessBoard() : ChessBoard(start_position) {}

/*
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

    //passant target, castling rights, turn are already default set

    stateStack[0].update_occupancies();
    
    ChessBoard(start_position);
}
*/


ChessBoard::~ChessBoard() {
    delete[] stateStack;
}
