#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "zobrist.h"
#include "board.h"

//hash table size
#define HASH_SIZE 0x400000

enum HashFlag {
    HASH_EXACT = 0,
    HASH_ALPHA = 1,
    HASH_BETA  = 2
};

struct TTEntry {
    uint64_t hash_key = 0;
    int depth = 0;
    int flag = HASH_EXACT;  //fail-low, fail-high, PV
    int score = 0;          //score (alpha/beta/PV)
    //Move best{};            //best move
};

//define TT
class TranspositionTable {
    public:
        void resize(size_t mb);
        void clear();
        void store(uint64_t key, int depth, int value, int flag, Move best);
        bool probe(uint64_t key, int depth, int alpha, int beta, int& value, Move& best) const;

    private:
        TTEntry table[HASH_SIZE];
};

#endif
