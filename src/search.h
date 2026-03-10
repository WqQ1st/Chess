#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "bitboards.h"
#include "eval.h"
#include "movegen.h"


static int negamax(ChessBoard& board, int alpha, int beta, int depth);

int search_position(ChessBoard& board, int depth);

#endif
