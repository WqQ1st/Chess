#include "zobrist.h"
#include "board.h"
#include "attacks.h"
#include <iostream>

uint64_t piece_keys[12][64];
uint64_t enpassant_keys[64];
uint64_t castle_keys[16];
uint64_t side_key;

//init random hash keys
void init_random_keys() {
    //update pseudo random number state
    //unsigned int random_state = 1804289383;

    //loop over piece codes
    for (int piece = WHITE_PAWN; piece <= BLACK_KING; ++piece) {
        //loop over board squares
        for (int square = 0; square < 64; ++square) {
            //init random piece keys
            piece_keys[piece][square] = get_random_u64_number();
            std::cout << piece_keys[piece][square] << std::endl;
        }
    }
}

