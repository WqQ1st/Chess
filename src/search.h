#ifndef SEARCH_H
#define SEARCH_H

#include "bitboards.h"
#include "eval.h"
#include "movegen.h"
#include "constants.h"
#include "board.h"

#include <cstdint>
#include <iostream>

//constants
extern bool follow_pv;
extern bool score_pv;
extern int pv_length[max_ply];
extern Move pv_table[max_ply][max_ply];
extern int ply;
extern int R;

extern const int full_depth_moves;
extern const int reduction_limit;

static void enable_pv_scoring(std::vector<Move>& moves);

int search_position(ChessBoard& board, int depth);

Move find_best_move(ChessBoard& board, int depth);

void search_and_print(ChessBoard& board, int depth);

void print_nodes();

static void clear_vars();

void clear_transposition_table();

void set_search_time_limit(uint64_t stop_time);

void clear_search_time_limit();

void stop_search_now();

void set_uci_info_output(bool enabled);

#endif
