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
                            moves.push_back(Move(source_square, target_square, piece, WHITE_QUEEN, MF_NONE));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_BISHOP, MF_NONE));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_KNIGHT, MF_NONE));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_ROOK, MF_NONE));
                        } else {
                            //pawn push
                            moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                            //double pawn push
                            if ((source_square >= A2 && source_square <= H2) && !get_bit(state.occupancies[BOTH], target_square - 8)) {
                                //add to move list
                                moves.push_back(Move(source_square, target_square - 8, piece, EMPTY, MF_DOUBLE));
                            }
                        }
                    }
                    //init pawn attacks bitboard
                    attacks = pawn_attacks[side][source_square] & state.occupancies[BLACK];

                    //generate pawn capture moves
                    while (attacks) {
                        //init target square
                        target_square = pop_lsb(attacks);

                        //capture is a promotion
                        if (source_square >= A7 && source_square <= H7) {
                            //add moves into move list
                            moves.push_back(Move(source_square, target_square, piece, WHITE_QUEEN, MF_CAPTURE));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_BISHOP, MF_CAPTURE));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_KNIGHT, MF_CAPTURE));
                            moves.push_back(Move(source_square, target_square, piece, WHITE_ROOK, MF_CAPTURE));
                        } else { //otherwise capture
                            moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                        }
                    }

                    //generate en passant squares
                    if (state.passantTarget != EMPTY) {
                        //lookup pawn attacks and bitwise AND with en passant square (bit)
                        uint64_t passant_attacks = pawn_attacks[side][source_square] & (1ULL << state.passantTarget);

                        //make sure en passant capture available
                        if (passant_attacks) {
                            //init en passant square
                            int target_enpassant = pop_lsb(passant_attacks);
                            moves.push_back(Move(source_square, target_enpassant, piece, EMPTY, MF_ENPASSANT));
                        }
                    }
                }
            }

            //castling
            if (piece == WHITE_KING) {
                //king side castling available
                if (state.castle & wk) {
                    //make sure squares between king and king's rook are empty
                    if (!get_bit(state.occupancies[BOTH], F1) && !get_bit(state.occupancies[BOTH], G1)) {
                        //make sure king and F1 squares aren't under attack
                        if (!is_square_attacked(E1, BLACK, state) && !is_square_attacked(F1, BLACK, state)) {
                            //king side castle
                            moves.push_back(Move(E1, G1, piece, EMPTY, MF_CASTLE));
                        }
                    }
                }
                //queen side castling available
                if (state.castle & wq) {
                    //make sure squares between king and queen's rook are empty
                    if (!get_bit(state.occupancies[BOTH], D1) && !get_bit(state.occupancies[BOTH], C1) && !get_bit(state.occupancies[BOTH], B1)) {
                        //make sure king and D1 squares aren't under attack
                        if (!is_square_attacked(E1, BLACK, state) && !is_square_attacked(D1, BLACK, state)) {
                            //queen side castle
                            moves.push_back(Move(E1, C1, piece, EMPTY, MF_CASTLE));
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
                            moves.push_back(Move(source_square, target_square, piece, BLACK_QUEEN, MF_NONE));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_BISHOP, MF_NONE));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_KNIGHT, MF_NONE));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_ROOK, MF_NONE));
                        } else {
                            //pawn push
                            moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                            //double pawn push
                            if ((source_square >= A7 && source_square <= H7) && !get_bit(state.occupancies[BOTH], target_square + 8)) {
                                //add to move list
                                moves.push_back(Move(source_square, target_square + 8, piece, EMPTY, MF_DOUBLE));
                            }
                        }
                    }
                    //init pawn attacks bitboard
                    attacks = pawn_attacks[side][source_square] & state.occupancies[WHITE];

                    //generate pawn capture moves
                    while (attacks) {
                        //init target square
                        target_square = pop_lsb(attacks);

                        //capture is a promotion
                        if (source_square >= A2 && source_square <= H2) {
                            //add moves into move list
                            moves.push_back(Move(source_square, target_square, piece, BLACK_QUEEN, MF_CAPTURE));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_BISHOP, MF_CAPTURE));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_KNIGHT, MF_CAPTURE));
                            moves.push_back(Move(source_square, target_square, piece, BLACK_ROOK, MF_CAPTURE));
                        } else { //otherwise capture
                            moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                        }
                    }

                    //generate en passant squares
                    if (state.passantTarget != EMPTY) {
                        //lookup pawn attacks and bitwise AND with en passant square (bit)
                        uint64_t passant_attacks = pawn_attacks[side][source_square] & (1ULL << state.passantTarget);

                        //make sure en passant capture available
                        if (passant_attacks) {
                            //init en passant square
                            int target_enpassant = pop_lsb(passant_attacks);
                            moves.push_back(Move(source_square, target_enpassant, piece, EMPTY, MF_ENPASSANT));
                        }
                    }
                }
            }

            //castling
            if (piece == BLACK_KING) {
                //king side castling available
                if (state.castle & bk) {
                    //make sure squares between king and king's rook are empty
                    if (!get_bit(state.occupancies[BOTH], F8) && !get_bit(state.occupancies[BOTH], G8)) {
                        //make sure king and F8 squares aren't under attack
                        if (!is_square_attacked(E8, WHITE, state) && !is_square_attacked(F8, WHITE, state)) {
                            //king side castle
                            moves.push_back(Move(E8, G8, piece, EMPTY, MF_CASTLE));
                        }
                    }
                }
                //queen side castling available
                if (state.castle & bq) {
                    //make sure squares between king and queen's rook are empty
                    if (!get_bit(state.occupancies[BOTH], D8) && !get_bit(state.occupancies[BOTH], C8) && !get_bit(state.occupancies[BOTH], B8)) {
                        //make sure king and D8 squares aren't under attack
                        if (!is_square_attacked(E8, WHITE, state) && !is_square_attacked(D8, WHITE, state)) {
                            //queen side castle
                            moves.push_back(Move(E8, C8, piece, EMPTY, MF_CASTLE));
                        }
                    }
                }
            }
        }

        //generate knight moves
        if ((state.turn == WHITE) ? piece == WHITE_KNIGHT : piece == BLACK_KNIGHT) {
            //loop over source squares of piece bitboard copy
            while (bitboard) {
                //init source square
                source_square = pop_lsb(bitboard);

                //init piece attacks for set of target squares
                attacks = knight_attacks[source_square] & ((state.turn == WHITE) ? ~state.occupancies[WHITE] : ~state.occupancies[BLACK]);

                //loop over target squares to generate moves
                while (attacks) {
                    //init target square
                    target_square = pop_lsb(attacks);

                    //quiet moves
                    if (!get_bit(((state.turn == WHITE) ? state.occupancies[BLACK] : state.occupancies[WHITE]), target_square)){
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                    } else { //capture moves
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                    }
                }
            }
        }

        //generate bishop moves
        if ((state.turn == WHITE) ? piece == WHITE_BISHOP : piece == BLACK_BISHOP) {
            //loop over source squares of piece bitboard copy
            while (bitboard) {
                //init source square
                source_square = pop_lsb(bitboard);

                //init piece attacks for set of target squares
                attacks = get_bishop_attacks(source_square, state.occupancies[BOTH]) & ((state.turn == WHITE) ? ~state.occupancies[WHITE] : ~state.occupancies[BLACK]);

                //loop over target squares to generate moves
                while (attacks) {
                    //init target square
                    target_square = pop_lsb(attacks);

                    //quiet moves
                    if (!get_bit(((state.turn == WHITE) ? state.occupancies[BLACK] : state.occupancies[WHITE]), target_square)){
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                    } else { //capture moves
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                    }
                }
            }
        }

        //generate rook moves
        if ((state.turn == WHITE) ? piece == WHITE_ROOK : piece == BLACK_ROOK) {
            //loop over source squares of piece bitboard copy
            while (bitboard) {
                //init source square
                source_square = pop_lsb(bitboard);

                //init piece attacks for set of target squares
                attacks = get_rook_attacks(source_square, state.occupancies[BOTH]) & ((state.turn == WHITE) ? ~state.occupancies[WHITE] : ~state.occupancies[BLACK]);

                //loop over target squares to generate moves
                while (attacks) {
                    //init target square
                    target_square = pop_lsb(attacks);

                    //quiet moves
                    if (!get_bit(((state.turn == WHITE) ? state.occupancies[BLACK] : state.occupancies[WHITE]), target_square)){
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                    } else { //capture moves
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                    }
                }
            }
        }

        //generate queen moves
        if ((state.turn == WHITE) ? piece == WHITE_QUEEN : piece == BLACK_QUEEN) {
            //loop over source squares of piece bitboard copy
            while (bitboard) {
                //init source square
                source_square = pop_lsb(bitboard);

                //init piece attacks for set of target squares
                attacks = get_queen_attacks(source_square, state.occupancies[BOTH]) & ((state.turn == WHITE) ? ~state.occupancies[WHITE] : ~state.occupancies[BLACK]);

                //loop over target squares to generate moves
                while (attacks) {
                    //init target square
                    target_square = pop_lsb(attacks);

                    //quiet moves
                    if (!get_bit(((state.turn == WHITE) ? state.occupancies[BLACK] : state.occupancies[WHITE]), target_square)){
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                    } else { //capture moves
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                    }
                }
            }
        }
        
        //generate king moves (non-castling)
        if ((state.turn == WHITE) ? piece == WHITE_KING : piece == BLACK_KING) {
            //loop over source squares of piece bitboard copy
            while (bitboard) {
                //init source square
                source_square = pop_lsb(bitboard);

                //init piece attacks for set of target squares
                attacks = king_attacks[source_square] & ((state.turn == WHITE) ? ~state.occupancies[WHITE] : ~state.occupancies[BLACK]);

                //loop over target squares to generate moves
                while (attacks) {
                    //init target square
                    target_square = pop_lsb(attacks);

                    //quiet moves
                    if (!get_bit(((state.turn == WHITE) ? state.occupancies[BLACK] : state.occupancies[WHITE]), target_square)){
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_NONE));
                    } else { //capture moves
                        moves.push_back(Move(source_square, target_square, piece, EMPTY, MF_CAPTURE));
                    }
                }
            }
        }
    }
}
