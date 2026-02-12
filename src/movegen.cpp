#include "movegen.h"

void generate_moves(const BoardState& state, std::vector<Move>& moves) {
    /*
    std::cout
    << "turn=" << int(state.turn)
    << " WP=" << state.bitboards[WHITE_PAWN]
    << " BP=" << state.bitboards[BLACK_PAWN]
    << " occ=" << state.occupancies[BOTH]
    << "\n";
    */

    //define source and target squares
    int source_square, target_square;

    //define current piece's bitboards copy and its attacks
    uint64_t bitboard, attacks;

    int side = state.turn;

    //depending on if black or white
    int start = (side == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    int end = start + 6;


    //loop over all bitboards
    for (int piece = start; piece < end; ++piece) {
        //init piece bitboard copy
        bitboard = state.bitboards[piece];

        //generate white pawns and white castling moves
        if (side == WHITE) {
            if (piece == WHITE_PAWN) {
                //loop over white pawns within bitboard
                while (bitboard) { //deletes bits along the way
                    //init source square
                    source_square = pop_lsb(bitboard);
                    //std::cout << "square: " << source_square << std::endl;

                    //init target square
                    target_square = source_square - 8;

                    //generate quiet pawn moves
                    if ((target_square >= 0) && !get_bit(state.occupancies[BOTH], target_square)) { //if the target is on the board
                        //pawn promotion
                        if (source_square >= A7 && source_square <= H7) {
                            //add moves into move list
                            moves.push_back(Move(source_square, target_square, piece, WHITE_QUEEN));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_BISHOP));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_KNIGHT));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_ROOK));
                        } else {
                            //pawn push
                            moves.push_back(Move(source_square, target_square, piece, EMPTY));
                            //double pawn push
                            if ((source_square >= A2 && source_square <= H2) && !get_bit(state.occupancies[BOTH], target_square - 8)) {
                                //add to move list
                                moves.push_back(Move(source_square, target_square - 8, piece, EMPTY));
                            }
                        }
                    }
                }
            }
        } else { //generate black pawns and black castling moves
            if (piece == BLACK_PAWN) {
                //loop over black pawns within bitboard
                while (bitboard) { //deletes bits along the way
                    //init source square
                    source_square = pop_lsb(bitboard);
                    //std::cout << "square: " << source_square << std::endl;

                    //init target square
                    target_square = source_square + 8;

                    //generate quiet pawn moves
                    if ((target_square < 64) && !get_bit(state.occupancies[BOTH], target_square)) { //if the target is on the board
                        //pawn promotion
                        if (source_square >= A2 && source_square <= H2) {
                            //add moves into move list
                            moves.push_back(Move(source_square, target_square, piece, BLACK_QUEEN));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_BISHOP));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_KNIGHT));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_ROOK));
                        } else {
                            //pawn push
                            moves.push_back(Move(source_square, target_square, piece, EMPTY));
                            //double pawn push
                            if ((source_square >= A7 && source_square <= H7) && !get_bit(state.occupancies[BOTH], target_square + 8)) {
                                //add to move list
                                moves.push_back(Move(source_square, target_square + 8, piece, EMPTY));
                            }
                        }
                    }
                }
            }
        }

        //generate knight moves

        //generate bishop moves

        //generate rook moves

        //generate queen moves
        
        //generate king moves (non-castling)

    }
}
