#include "transposition.h"

void TranspositionTable::resize(size_t mb) {

}

//clear TT
void TranspositionTable::clear() {
    //loop over all TT elements
    for (int index = 0; index < HASH_SIZE; ++index) {
        //clear all attributes
        table[index].hash_key = 0;
        table[index].depth = 0;
        table[index].flag = 0;
        table[index].score = 0;
    }
}

void TranspositionTable::store(uint64_t key, int depth, int value, int flag, Move best) {

}

bool TranspositionTable::probe(uint64_t key, int depth, int alpha, int beta, int& value, Move& best) const {
    return false;
}
