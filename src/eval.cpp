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

            //score material weights
            score += material_score[bb_piece];

            //score positional piece scores
            switch (bb_piece) {
                //evaluate white pieces' positional scores
                case WHITE_PAWN:    score += pawn_score[square]; break;
                case WHITE_KNIGHT:  score += knight_score[square]; break;
                case WHITE_BISHOP:  score += bishop_score[square]; break;
                case WHITE_ROOK:    score += rook_score[square]; break;
                case WHITE_KING:    score += king_score[square]; break;

                //evaluate black pieces' positional scores
                case BLACK_PAWN:    score -= pawn_score[mirror_score[square]]; break;
                case BLACK_KNIGHT:  score -= knight_score[mirror_score[square]]; break;
                case BLACK_BISHOP:  score -= bishop_score[mirror_score[square]]; break;
                case BLACK_ROOK:    score -= rook_score[mirror_score[square]]; break;
                case BLACK_KING:    score -= king_score[mirror_score[square]]; break;
            }
        }
    }

    //positive score means current side is better
    return (board.curr_state().turn == WHITE) ? score : -score;
}
