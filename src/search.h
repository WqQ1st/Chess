#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "bitboards.h"
#include "eval.h"
#include "movegen.h"

#include <iostream>

int search_position(ChessBoard& board, int depth);

Move find_best_move(ChessBoard& board, int depth);

void print_nodes();

#endif
