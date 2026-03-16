#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "bitboards.h"
#include "eval.h"
#include "movegen.h"

#include <iostream>

int search_position(ChessBoard& board, int depth);

Move find_best_move(ChessBoard& board, int depth);

void search_and_print(ChessBoard& board, int depth);

void print_nodes();

extern int pv_length[64];

extern Move pv_table[64][64];

#endif
