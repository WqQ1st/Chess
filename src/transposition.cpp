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

void TranspositionTable::store(uint64_t key, int depth, int value, int flag) {
    TTEntry& e = table[key % HASH_SIZE];

    //if TT entry is already deeper
    if (e.hash_key == key && e.depth > depth) {
        return;
    }
    
    //overwrite hash entry data
    e.hash_key = key;
    e.depth = depth;
    e.score = value;
    e.flag = flag;
}

bool TranspositionTable::probe(uint64_t key, int depth, int alpha, int beta, int& value) const {
    //create reference to hash entry
    const TTEntry& e = table[key % HASH_SIZE];
    
    //ensure the position matches
    if (e.hash_key == key && e.depth >= depth) {
        //PV node score
        if (e.flag == HASH_EXACT || (e.flag == HASH_ALPHA && e.score <= alpha) || (e.flag == HASH_BETA && e.score >= beta)) {
            value = e.score;
            return true;
        }
        /*
        //fail-low
        if (e.flag == HASH_ALPHA && e.score <= alpha) {
            value = alpha;
            return true;
        }
        //fail-high
        if (e.flag == HASH_BETA && e.score >= beta) {
            value = beta;
            return true;
        }
        */
    }
    //entry doesn't exist
    return false;
}
