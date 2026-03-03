#include "eval.h"

int evaluate(const ChessBoard& board) {
    //static evaluation score
    int score = 0;

    //init temp variables to store all pieces on the board
    uint64_t bitboard;
    int square;

    //stores pointer to current state
    const BoardState& state = board.curr_state();

    //loop over all piece bitboards
    for (int bb_piece = WHITE_PAWN; bb_piece <= BLACK_KING; ++bb_piece) {
        //bitboard copy
        bitboard = state.bitboards[bb_piece];

        while (bitboard) {
            //init square
            square = pop_lsb(bitboard);

            //add score of the piece
            score += material_score[bb_piece];
        }
    }

    //positive score means current side is better
    return (board.curr_state().turn == WHITE) ? score : -score;
}
