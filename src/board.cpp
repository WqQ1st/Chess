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

//make move on board
void ChessBoard::move(const Move& move) {

    //distinguish btwn quiet and capture moves

    //push a new state onto the stack
    stackIndex++;
    stateStack[stackIndex] = stateStack[stackIndex - 1];

    uint64_t fromBoard = uint64_t(1) << move.from();
    uint64_t toBoard = uint64_t(1) << move.to();
    uint64_t moveBoard = fromBoard | toBoard;

    //move a piece and store which type (ex: white pawn)
    uint8_t movedPiece = move.piece();

    auto& st = stateStack[stackIndex];

    st.bitboards[movedPiece] ^= (fromBoard | toBoard);

    int capturedPiece = EMPTY;

    //capture a piece
    for (int i = st.turn == WHITE ? 6 : 0; i < (st.turn == WHITE ? 11 : 5); ++i) {
        if (st.bitboards[i] & toBoard) {
            st.bitboards[i] ^= toBoard;
            capturedPiece = i;
            break;
        }
    }

    //en-passant
    if (toBoard == st.passantTarget) {
        if (movedPiece == WHITE_PAWN) {
            st.bitboards[BLACK_PAWN] ^= st.passantTarget << 8;
        } else if (movedPiece == BLACK_PAWN) {
            st.bitboards[WHITE_PAWN] ^= st.passantTarget >> 8;
        }
    }

    //set en-passant target
    if ((movedPiece == WHITE_PAWN || movedPiece == BLACK_PAWN) && abs(move.from() - move.to()) == 16) {
        st.passantTarget = uint64_t(1) << int((move.from() + move.to()) * 0.5);
    } else {
        st.passantTarget = 0;
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

    

    // castle move: king moved two squares, so move rook too; generate moves alr checks legality of castling
    if (move.flags() & MF_CASTLE) {
        if (movedPiece == WHITE_KING) {
            if (move.to() == G1) st.bitboards[WHITE_ROOK] ^= (BB(H1) | BB(F1));
            else if (move.to() == C1) st.bitboards[WHITE_ROOK] ^= (BB(A1) | BB(D1));
        } else if (movedPiece == BLACK_KING) {
            if (move.to() == G8) st.bitboards[BLACK_ROOK] ^= (BB(H8) | BB(F8));
            else if (move.to() == C8) st.bitboards[BLACK_ROOK] ^= (BB(A8) | BB(D8));
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
    if (move.promotion() != EMPTY) {
        st.bitboards[movedPiece] ^= toBoard;          // remove pawn that just landed
        st.bitboards[move.promotion()] ^= toBoard;    // add promoted piece
    }

    //switch turns
    if (st.turn == WHITE) {
        st.turn = BLACK;
    } else {
        st.turn = WHITE;
    }

    st.update_occupancies();
}

void ChessBoard::undo() {
    if (stackIndex > 0) {
        --stackIndex;
    }
}

//returns true and applies the move if legal, otherwise returns false
bool ChessBoard::try_move(const Move& m) {
    // side that is about to move
    int us = curr_state().turn;
    int them = (us == WHITE) ? BLACK : WHITE;

    move(m); // flips turn internally

    // find our king square in the NEW state
    const BoardState& st = curr_state();
    uint64_t kingBB = st.bitboards[(us == WHITE) ? WHITE_KING : BLACK_KING];

    // should never happen, but be defensive
    if (!kingBB) {
        undo();
        return false;
    }

    int kingSq = __builtin_ctzll(kingBB); // takes lsb since exactly 1 king bit should be set

    // if opponent attacks our king, illegal
    if (is_square_attacked(kingSq, them, st)) {
        undo();
        return false;
    }

    return true;
}

//parse FEN string
BoardState ChessBoard::parse_fen(const char* fen) {
    BoardState s{};

    int rank = 0; // 0 is rank 8
    int file = 0;

    while (rank < 8 && *fen) {
        char c = *fen;

        if (c == '/') {
            rank++;
            file = 0;
            fen++;
            continue;
        }

        if (c >= '1' && c <= '8') {
            file += (c - '0');
            fen++;
            continue;
        }

        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) {
            int piece = char_pieces[c];
            int square = rank * 8 + file;
            s.bitboards[piece] |= (1ULL << square);
            file++;
            fen++;
            continue;
        }

        if (c == ' ') break;
        fen++;
    }

    while (*fen == ' ') fen++;

    s.turn = (*fen == 'w') ? WHITE : BLACK;
    while (*fen && *fen != ' ') fen++;
    while (*fen == ' ') fen++;

    s.castle = 0;
    while (*fen && *fen != ' ') {
        if (*fen == 'K') s.castle |= wk;
        else if (*fen == 'Q') s.castle |= wq;
        else if (*fen == 'k') s.castle |= bk;
        else if (*fen == 'q') s.castle |= bq;
        fen++;
    }
    while (*fen == ' ') fen++;

    s.passantTarget = NO_SQ;
    if (*fen != '-') {
        int epFile = fen[0] - 'a';
        int epRank = 8 - (fen[1] - '0'); // a8 is 0
        s.passantTarget = epRank * 8 + epFile; // store square index
        fen += 2;
    } else {
        fen++;
    }

    s.update_occupancies();
    return s;
}


BoardState& ChessBoard::curr_state() {
    return stateStack[stackIndex];
}

ChessBoard::ChessBoard(const char *fen) {
    stateStack = new BoardState[1000];
    stackIndex = 0;

    stateStack[0] = parse_fen(fen);
    //parse fen should alr update occupancies
    stateStack[stackIndex].update_occupancies();
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
