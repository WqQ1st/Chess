// uci_main.cpp
#include "uci.h"
#include "attacks.h"
#include "zobrist.h"

void init_all() {
    //initialize leaper atk tables
    init_leapers_attacks();
    
    //init_magic_numbers();

    init_sliders_attacks(true);
    init_sliders_attacks(false);

    //init random keys
    init_random_keys();
}

int main() {
    init_all();

    uci_loop();
}
