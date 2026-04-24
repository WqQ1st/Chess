#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "zobrist.h"
#include "board.h"

enum HashFlag {
    HASH_EXACT = 0,
    HASH_ALPHA = 1,
    HASH_BETA  = 2
};

struct TTEntry {
    uint64_t key = 0;
    int depth = 0;
    int flag = HASH_EXACT;
    int value = 0;
    Move best{};
};



#endif
