#ifndef ATTACKS_H
#define ATTACKS_H

#include <cstdint>
#include "board.h"
#include "bitboards.h"

using std::uint8_t;
using std::uint64_t;

extern uint64_t pawnAttacks[2][64];
extern uint64_t knightAttacks[64];
extern uint64_t kingAttacks[64];

//initialize attack boards
void init_leapers_attacks();

#endif
