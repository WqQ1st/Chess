#ifndef SEARCH_H
#define SEARCH_H

#include "board.h"
#include "bitboards.h"
#include "eval.h"
#include "movegen.h"
#include "constants.h"

#include <iostream>

int search_position(ChessBoard& board, int depth);

Move find_best_move(ChessBoard& board, int depth);

void search_and_print(ChessBoard& board, int depth);

void print_nodes();

static void clear_vars();

extern int pv_length[max_ply];

extern Move pv_table[max_ply][max_ply];

#endif
