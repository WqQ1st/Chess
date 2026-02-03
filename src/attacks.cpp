#include "attacks.h"
#include <math.h>

using std::uint8_t;
using std::uint64_t;

uint64_t pawnAttacks[2][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];

static uint64_t mask_pawn_attacks(uint8_t square, int side) {
    uint64_t b = 1ULL << square;

    if (side == WHITE) {
        return ((b >> 7) & NOT_A_FILE) |  // up-right (kills wrap to A-file)
               ((b >> 9) & NOT_H_FILE);   // up-left
    } else {
        return ((b << 7) & NOT_H_FILE) |  // down-left
               ((b << 9) & NOT_A_FILE);   // down-right
    }
}

static uint64_t mask_knight_attacks(uint8_t square) {
    uint64_t b = 1ULL << square;
    uint64_t a = 0;

    a |= (b << 17) & NOT_A_FILE;   // 2 south, 1 west
    a |= (b << 15) & NOT_H_FILE;   // 2 south, 1 east
    a |= (b << 10) & NOT_AB_FILE;  // 1 south, 2 west
    a |= (b << 6)  & NOT_GH_FILE;  // 1 south, 2 east

    a |= (b >> 17) & NOT_H_FILE;   // 2 north, 1 east
    a |= (b >> 15) & NOT_A_FILE;   // 2 north, 1 west
    a |= (b >> 10) & NOT_GH_FILE;  // 1 north, 2 east
    a |= (b >> 6)  & NOT_AB_FILE;  // 1 north, 2 west

    return a;
}

void init_leapers_attacks() {
    for (int sq = 0; sq < 64; ++sq) {
        pawnAttacks[WHITE][sq] = mask_pawn_attacks(sq, WHITE);
        pawnAttacks[BLACK][sq] = mask_pawn_attacks(sq, BLACK);
        knightAttacks[sq] = mask_knight_attacks(sq);
    }
}
