#ifndef ZOBRIST_H
#define ZOBRIST_H

#include <cstdint>

struct BoardState;

//random piece keys [piece][square]
extern uint64_t piece_keys[12][64];
//random enpassant keys [square]
extern uint64_t enpassant_keys[64];
//random castling keys
extern uint64_t castle_keys[16];
//random side key
extern uint64_t side_key;

void init_random_keys();

#endif
