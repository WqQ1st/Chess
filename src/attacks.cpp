#include "attacks.h"
#include <math.h>
#include "board.h"
//board.h has "WHITE", "BLACK" enums 
#include <string.h>
#include <iostream>

using std::uint8_t;
using std::uint64_t;

//initialize attack tables
uint64_t pawn_attacks[2][64];
uint64_t knight_attacks[64];
uint64_t king_attacks[64];
uint64_t bishop_attacks[64][512];
uint64_t rook_attacks[64][4096];

//init masks
uint64_t bishop_masks[64];
uint64_t rook_masks[64];

//init state (random number)
unsigned int random_state = 1804289383;

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


//generate bishop atks on the fly
static uint64_t bishop_attacks_on_the_fly(uint8_t square, uint64_t block) {
    uint64_t attacks = 0;

    //rank, file
    int r, f;

    //target rank and files
    int tr = square / 8;
    int tf = square % 8;

    //generate bishop atks
    for (r = tr + 1, f = tf + 1; r <= 7 && f <= 7; ++r, ++f) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) {
            break;
        }
    }
    for (r = tr - 1, f = tf + 1; r >= 0 && f <= 7; --r, ++f) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) {
            break;
        }
    }
    for (r = tr + 1, f = tf - 1; r <= 7 && f >= 0; ++r, --f) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) {
            break;
        }
    }
    for (r = tr - 1, f = tf - 1; r >= 0 && f >= 0; --r, --f) {
        attacks |= (1ULL << (r * 8 + f));
        if ((1ULL << (r * 8 + f)) & block) {
            break;
        }
    }

    return attacks;
}


//generate rook atks on the fly
static uint64_t rook_attacks_on_the_fly(uint8_t square, uint64_t block) {
    uint64_t attacks = 0;

    //rank, file
    int r, f;

    //target rank and files
    int tr = square / 8;
    int tf = square % 8;

    //generate rook atks
    for (r = tr + 1; r <= 7; ++r) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) {
            break;
        }
    }
    for (r = tr - 1; r >= 0; --r) {
        attacks |= (1ULL << (r * 8 + tf));
        if ((1ULL << (r * 8 + tf)) & block) {
            break;
        }
    }
    for (f = tf + 1; f <= 7; ++f) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) {
            break;
        }
    }
    for (f = tf - 1; f >= 0; --f) {
        attacks |= (1ULL << (tr * 8 + f));
        if ((1ULL << (tr * 8 + f)) & block) {
            break;
        }
    }

    return attacks;
}

//set occupancies
uint64_t set_occupancy(int index, int bits_in_mask, uint64_t attack_mask) {
    //occupancy map
    uint64_t occupancy = 0ULL;

    //loop over the range of bits within attack mask
    for (int count = 0; count < bits_in_mask; ++count) {
        //pop LS1B index of attack mask
        int square = pop_lsb(attack_mask);

        //make sure occupancy is on board
        if (index & (1 << count)) {
            //populate occupancy map
            occupancy |= (1ULL << square);
        }
    }

    return occupancy;
}

//generate 32-bit pseudo legal numbers
unsigned int get_random_u32_number() {
    //get current state
    unsigned int number = random_state;

    //XOR shift algorithm
    number ^= number << 13;
    number ^= number >> 17;
    number ^= number << 5;

    //update random number state
    random_state = number;

    //return random number
    return number;
}

//generate 64-bit pseudo legal numbers
uint64_t get_random_u64_number() {
    //define 4 random numbers
    uint64_t n1, n2, n3, n4;

    //init random numbers slicing 16 bits from MS1B side
    n1 = (uint64_t)(get_random_u32_number()) & 0xFFFF;
    n2 = (uint64_t)(get_random_u32_number()) & 0xFFFF;
    n3 = (uint64_t)(get_random_u32_number()) & 0xFFFF;
    n4 = (uint64_t)(get_random_u32_number()) & 0xFFFF;

    //return random number
    return n1 | (n2 << 16) | (n3 << 32) | (n4 << 48);
}

//generate magic number candidate
uint64_t generate_magic_number() {
    return get_random_u64_number() & get_random_u64_number() & get_random_u64_number();
}

//find appropriate magic number
uint64_t find_magic_number(uint8_t square, int relevant_bits, bool bishop) {
    //init occupancies
    uint64_t occupancies[4096];

    //init attack tables
    uint64_t attacks[4096];
    
    //init used attacks
    uint64_t used_attacks[4096];

    //init attack mask for a current piece
    uint64_t attack_mask = bishop ? mask_bishop_attacks(square) : mask_rook_attacks(square);

    //initialize occupancy indices
    int occupancy_indices = 1 << relevant_bits;

    //loop over occupancy indices
    for (int index = 0; index < occupancy_indices; ++index) {
        //initialize occupancies
        occupancies[index] = set_occupancy(index, relevant_bits, attack_mask);

        //init attacks
        attacks[index] = bishop ? bishop_attacks_on_the_fly(square, occupancies[index]) : rook_attacks_on_the_fly(square, occupancies[index]);
    }
    //test magic numbers loop
    for (int random_count = 0; random_count < 10000000; ++random_count) {
        //generate magic number candidate
        uint64_t magic_number = generate_magic_number();

        //skip inappropriate magic numbers
        if (count_bits((attack_mask * magic_number) & 0xFF00000000000000ULL) < 6) {
            continue;
        }

        //init used attacks array
        memset(used_attacks, 0ULL, sizeof(used_attacks));

        //init index and fail flag
        int index, fail;

        //test magic index loop
        for (index = 0, fail = 0; !fail && index < occupancy_indices; ++index) {
            //init magic index
            int magic_index = (int)((occupancies[index] * magic_number) >> (64 - relevant_bits));

            //if magic index works
            if (used_attacks[magic_index] == 0ULL) {
                //init used attacks
                used_attacks[magic_index] = attacks[index];
            } else if (used_attacks[magic_index] != attacks[index]) {
                //magic index doesnt work
                fail = 1;
            }
        }

        if (!fail) {
            return magic_number;
        }
    }
    //if magic number doesnt work
    std::cout << "Magic number fails" << std::endl;
    return 0ULL;
}

/*
//init magic numbers
void init_magic_numbers() {
    //loop over 64 board squares
    for (int square = 0; square < 64; ++square) {
        //init rook magic numbers
        rook_magic_numbers[square] = find_magic_number(square, rook_relevant_bits[square], false);
    }

    for (int square = 0; square < 64; ++square) {
        //init bishop magic numbers
        bishop_magic_numbers[square] = find_magic_number(square, bishop_relevant_bits[square], true);
    }
}
*/

//init slider piece atk tables
void init_sliders_attacks(bool bishop) {
    for (int square = 0; square < 64; ++square) {
        //init bishop and rook masks
        bishop_masks[square] = mask_bishop_attacks(square);
        rook_masks[square] = mask_rook_attacks(square);

        //init current mask
        uint64_t attack_mask = bishop ? bishop_masks[square] : rook_masks[square];

        //relevant occupancy bit count
        int relevant_bits_count = count_bits(attack_mask);

        //init occupancy indices
        int occupancy_indices = (uint64_t(1) << relevant_bits_count);

        //loop over occupancy indices
        for (int index = 0; index < occupancy_indices; ++index) {
            if (bishop) {
                //current occupancy
                uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                //magic index
                int magic_index = (occupancy * bishop_magic_numbers[square]) >> (64 - bishop_relevant_bits[square]);

                bishop_attacks[square][magic_index] = bishop_attacks_on_the_fly(square, occupancy);
            } else {
                //current occupancy
                uint64_t occupancy = set_occupancy(index, relevant_bits_count, attack_mask);

                //magic index
                int magic_index = (occupancy * rook_magic_numbers[square]) >> (64 - rook_relevant_bits[square]);

                rook_attacks[square][magic_index] = rook_attacks_on_the_fly(square, occupancy);
            }
        }
    }
}

/*
//get bishop attacks
inline uint64_t get_bishop_attacks(uint8_t square, uint64_t occupancy) {
    //get bishop attacks given current occupancy
    occupancy &= bishop_masks[square];
    occupancy *= bishop_magic_numbers[square];
    occupancy >>= 64 - bishop_relevant_bits[square];
    
    return bishop_attacks[square][occupancy];
}


//get rook attacks
inline uint64_t get_rook_attacks(uint8_t square, uint64_t occupancy) {
    //get rook attacks given current occupancy
    occupancy &= rook_masks[square];
    occupancy *= rook_magic_numbers[square];
    occupancy >>= 64 - rook_relevant_bits[square];
    
    return rook_attacks[square][occupancy];
}

//get queen attacks
inline uint64_t get_queen_attacks(uint8_t square, uint64_t occupancy) {
    return (get_bishop_attacks(square, occupancy) | get_rook_attacks(square, occupancy));
}
*/

//is square attacked by the given side
int is_square_attacked(int square, int side, const BoardState& state) {

    const uint64_t *bitboards = state.bitboards;

    bool verbose = true;

    //attacked by white pawns
    if ((side == WHITE) && pawn_attacks[BLACK][square] & bitboards[WHITE_PAWN]) {
        if (verbose) std::cout << "attacked by pawn" << std::endl;
        return 1;
    }

    //attacked by black pawns
    if ((side == BLACK) && pawn_attacks[WHITE][square] & bitboards[BLACK_PAWN]) {
        if (verbose) std::cout << "attacked by pawn" << std::endl;
        return 1;
    }

    //attacked by knights
    if (knight_attacks[square] & ((side == WHITE) ? bitboards[WHITE_KNIGHT] : bitboards[BLACK_KNIGHT])) {
        if (verbose) std::cout << "attacked by knight" << std::endl;
        return 1;
    }

    //attacked by king
    if (king_attacks[square] & ((side == WHITE) ? bitboards[WHITE_KING] : bitboards[BLACK_KING])) {
        if (verbose) std::cout << "attacked by king" << std::endl;
        return 1;
    }

    //attacked by bishops
    if (get_bishop_attacks(square, state.occupancies[BOTH]) & ((side == WHITE) ? bitboards[WHITE_BISHOP] : bitboards[BLACK_BISHOP])) {
        if (verbose) std::cout << "attacked by bishop" << std::endl;
        return 1;
    }

    //attacked by rooks
    if (get_rook_attacks(square, state.occupancies[BOTH]) & ((side == WHITE) ? bitboards[WHITE_ROOK] : bitboards[BLACK_ROOK])) {
        if (verbose) std::cout << "attacked by rook" << std::endl;
        return 1;
    }

    //attacked by queens
    if (get_queen_attacks(square, state.occupancies[BOTH]) & ((side == WHITE) ? bitboards[WHITE_QUEEN] : bitboards[BLACK_QUEEN])) {
        if (verbose) std::cout << "attacked by queen" << std::endl;
        return 1;
    }

    return 0;
}

void init_leapers_attacks() {
    for (int sq = 0; sq < 64; ++sq) {
        pawn_attacks[WHITE][sq] = mask_pawn_attacks(sq, WHITE);
        pawn_attacks[BLACK][sq] = mask_pawn_attacks(sq, BLACK);
        knight_attacks[sq] = mask_knight_attacks(sq);
        king_attacks[sq] = mask_king_attacks(sq);
    }
}
