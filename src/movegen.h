#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <cstdint>
#include "bitboards.h"
#include "attacks.h"
#include <math.h>
#include "board.h"
#include "square.h"
//board.h has "WHITE", "BLACK" enums 
#include <string.h>
#include <iostream>
#include <vector>

using std::uint8_t;
using std::uint64_t;

enum class GenMode { All, CapturesOnly };

void generate_moves(const BoardState& state, std::vector<Move>& moves, GenMode mode);

#endif
