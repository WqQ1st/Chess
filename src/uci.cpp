#include "uci.h"

#include <iostream>
#include <string>
#include "board.h"

void uci_loop() {
    std::string line;

    while (std::getline(std::cin, line)) {
        if (line == "uci") {
            std::cout << "id name ChessEngine\n";
            std::cout << "id author Shane\n";
            std::cout << "uciok\n";
        } else if (line == "isready") {
            std::cout << "readyok\n";
        } else if (line == "quit") {
            break;
        }
    }
}
