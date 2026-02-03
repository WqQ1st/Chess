#include "attacks.h"
#include <math.h>

using std::uint8_t;
using std::uint64_t;

//initialize attack tables
uint64_t pawnAttacks[2][64];
uint64_t knightAttacks[64];
uint64_t kingAttacks[64];

//generate pawn atk board
static uint64_t mask_pawn_attacks(uint8_t square, int side) {
    uint64_t bitboard = 1ULL << square;

    if (side == WHITE) {
        return ((bitboard >> 7) & NOT_A_FILE) |  // up-right (kills wrap to A-file)
               ((bitboard >> 9) & NOT_H_FILE);   // up-left
    } else {
        return ((bitboard << 7) & NOT_H_FILE) |  // down-left
               ((bitboard << 9) & NOT_A_FILE);   // down-right
    }
}

//generate knight atk board
static uint64_t mask_knight_attacks(uint8_t square) {
    uint64_t bitboard = 1ULL << square;
    uint64_t attacks = 0;

    // south moves
    attacks|= (bitboard << 17) & NOT_A_FILE;    // 2 south, 1 east
    attacks|= (bitboard << 15) & NOT_H_FILE;    // 2 south, 1 west
    attacks|= (bitboard << 10) & NOT_AB_FILE;   // 1 south, 2 east
    attacks|= (bitboard << 6)  & NOT_GH_FILE;   // 1 south, 2 west

    // north moves
    attacks|= (bitboard >> 17) & NOT_H_FILE;    // 2 north, 1 west
    attacks|= (bitboard >> 15) & NOT_A_FILE;    // 2 north, 1 east
    attacks|= (bitboard >> 10) & NOT_GH_FILE;   // 1 north, 2 west
    attacks|= (bitboard >> 6)  & NOT_AB_FILE;   // 1 north, 2 east



    return attacks;
}

//generate king atk board
static uint64_t mask_king_attacks(uint8_t square) {
    uint64_t bitboard = 1ULL << square;
    uint64_t attacks= 0;

    attacks|= (bitboard << 8);                         // south
    attacks|= (bitboard >> 8);                         // north

    attacks|= (bitboard << 1) & NOT_A_FILE;            // east
    attacks|= (bitboard >> 1) & NOT_H_FILE;            // west

    attacks|= (bitboard << 9) & NOT_A_FILE;            // south east
    attacks|= (bitboard << 7) & NOT_H_FILE;            // south west

    attacks|= (bitboard >> 7) & NOT_A_FILE;            // north east
    attacks|= (bitboard >> 9) & NOT_H_FILE;            // north west

    return attacks;
}

//generate bishop atk board
static uint64_t mask_bishop_attacks(uint8_t square) {
    uint64_t attacks = 0;

    //rank, file
    int r, f;

    //target rank and files
    int tr = square / 8;
    int tf = square % 8;

    //mask relevant bishop occupancy bits
    for (r = tr + 1, f = tf + 1; r <= 6 && f <= 6; ++r, ++f) {
        attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf + 1; r >= 1 && f <= 6; --r, ++f) {
        attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr + 1, f = tf - 1; r <= 6 && f >= 1; ++r, --f) {
        attacks |= (1ULL << (r * 8 + f));
    }
    for (r = tr - 1, f = tf - 1; r >= 1 && f >= 1; --r, --f) {
        attacks |= (1ULL << (r * 8 + f));
    }

    return attacks;
}

//generate rook atk board
static uint64_t mask_rook_attacks(uint8_t square) {
    uint64_t attacks = 0;

    //rank, file
    int r, f;

    //target rank and files
    int tr = square / 8;
    int tf = square % 8;

    //mask relevant rook occupancy bits
    for (r = tr + 1; r <= 6; ++r) {
        attacks |= (1ULL << (r * 8 + tf));
    }
    for (r = tr - 1; r >= 1; --r) {
        attacks |= (1ULL << (r * 8 + tf));
    }
    for (f = tf + 1; f <= 6; ++f) {
        attacks |= (1ULL << (tr * 8 + f));
    }
    for (f = tf - 1; f >= 1; --f) {
        attacks |= (1ULL << (tr * 8 + f));
    }

    return attacks;
}

void init_leapers_attacks() {
    for (int sq = 0; sq < 64; ++sq) {
        pawnAttacks[WHITE][sq] = mask_pawn_attacks(sq, WHITE);
        pawnAttacks[BLACK][sq] = mask_pawn_attacks(sq, BLACK);
        knightAttacks[sq] = mask_knight_attacks(sq);
        kingAttacks[sq] = mask_king_attacks(sq);
    }
}
