#ifndef BITBOARDS_H
#define BITBOARDS_H

#include <cstdint>

using std::uint8_t;
using std::uint64_t;

// file masks
constexpr uint64_t A_FILE  = 0x0101010101010101ULL;
constexpr uint64_t B_FILE  = 0x0202020202020202ULL;
constexpr uint64_t G_FILE  = 0x4040404040404040ULL;
constexpr uint64_t H_FILE  = 0x8080808080808080ULL;

// inverse masks
constexpr uint64_t NOT_A_FILE  = 0xfefefefefefefefeULL;
constexpr uint64_t NOT_H_FILE  = 0x7f7f7f7f7f7f7f7fULL;
constexpr uint64_t NOT_AB_FILE = 0xfcfcfcfcfcfcfcfcULL;
constexpr uint64_t NOT_GH_FILE = 0x3f3f3f3f3f3f3f3fULL;


static inline int count_bits(uint64_t bitboard) {
    return __builtin_popcountll(bitboard);
}

static inline int get_ls1b_index(uint64_t bitboard) {
    return __builtin_ctzll(bitboard);
}

static inline int pop_lsb(uint64_t& bb)
{
    int sq = __builtin_ctzll(bb);
    bb &= bb - 1;
    return sq;
}

static inline int get_bit(uint64_t bitboard, int square) {
    return (bitboard >> square) & 1ULL;
}


#endif
