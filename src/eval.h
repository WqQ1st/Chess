#ifndef EVAL_H
#define EVAL_H

#include "board.h"
#include "bitboards.h"

inline constexpr int material_score[12] = {
    100,    //white pawn
    300,    //white knight
    350,    //white bishop
    500,    //white rook
    1000,   //white queen
    10000,  //white king
    -100,   //black pawn
    -300,   //black knight
    -350,   //black bishop
    -500,   //black rook
    -1000,  //black queen
    -10000  //black king
};

//position evaluation
int evaluate(const ChessBoard& board);

#endif
