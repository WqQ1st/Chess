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
    #include <string>

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

    enum MoveFlag : uint8_t {
        MF_NONE      = 0,
        MF_CAPTURE   = 1 << 0,
        MF_DOUBLE    = 1 << 1,
        MF_ENPASSANT = 1 << 2,
        MF_CASTLE    = 1 << 3
    };

    struct Move {
        /*
            binary move bits                                     hexidecimal constants
    
            0000 0000 0000 0000 0011 1111    source square       0x3f
            0000 0000 0000 1111 1100 0000    target square       0xfc0
            0000 0000 1111 0000 0000 0000    piece               0xf000
            0000 1111 0000 0000 0000 0000    promoted piece      0xf0000
            0001 0000 0000 0000 0000 0000    capture flag        0x100000
            0010 0000 0000 0000 0000 0000    double push flag    0x200000
            0100 0000 0000 0000 0000 0000    enpassant flag      0x400000
            1000 0000 0000 0000 0000 0000    castling flag       0x800000
        */

        uint32_t move = 0;

        Move(uint8_t source, uint8_t target, uint8_t piece, uint8_t promoted, MoveFlag flags) {
            move = (source) | (target << 6) | (piece << 12) | (promoted << 16) | (flags << 20);
        }


        uint8_t from;
        uint8_t to;

        int capture; //0: no capture, 1: capture

        uint8_t piece;      // moving piece (WHITE_PAWN, etc.)

        uint8_t promotion;  // promoted piece or EMPTY

        Move() = default;

        Move(uint8_t f, uint8_t t, uint8_t p, uint8_t promo = EMPTY)
        : from(f), to(t), piece(p), promotion(promo) {}


    std::string to_string() const {
            auto square_to_string = [](uint8_t sq) {
                char file = 'a' + (sq % 8);
                char rank = '8' - (sq / 8);
                return std::string{file} + rank;
            };

            std::string result = square_to_string(from) + square_to_string(to);

            if (promotion != EMPTY) {
                char promoChar = 'q';
                if (promotion == WHITE_ROOK || promotion == BLACK_ROOK) promoChar = 'r';
                else if (promotion == WHITE_BISHOP || promotion == BLACK_BISHOP) promoChar = 'b';
                else if (promotion == WHITE_KNIGHT || promotion == BLACK_KNIGHT) promoChar = 'n';

                result += promoChar;
            }

            return result;
        }
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

            occupancies[BOTH] = occupancies[WHITE] | occupancies[BLACK];
        }

        int piece_on(uint8_t square) const {
            uint64_t mask = 1ULL << square;
            for (int i = 0; i < 12; ++i) {
                if (bitboards[i] & mask) return i;
            }
            return EMPTY;
        }
    };

    class ChessBoard {
        private:
            //stack of board states
            BoardState* stateStack;
            int stackIndex;

        public:
            void print_bitboard(uint64_t bitboard);
            void print_bitboard();
            void print_occupancy();

            BoardState parse_fen(const char *fen);

            //setters and getters on a specific square
            uint8_t getPiece(uint8_t square);
            void setPiece(uint8_t piece, uint8_t square);

            //move and undo
            void move(const Move& move);
            void undo();

            BoardState& curr_state();

        //constructor
        ChessBoard();
        ChessBoard(const char *fen);

        //destructor
        ~ChessBoard();
    };

    #endif
