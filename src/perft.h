#ifndef PERFT_H
#define PERFT_H

#include "board.h"
#include "movegen.h"

uint64_t perft(ChessBoard& board, int depth);

#endif
