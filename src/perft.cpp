#include "perft.h"

//checks number of nodes
uint64_t perft(ChessBoard& board, int depth) {
    if (depth == 0) {
        return 1;
    }

    std::vector<Move> all_moves;
    //generate all legal moves
    board.generate_legal_moves(all_moves);

    uint64_t nodes = 0;
    for (const Move& m : all_moves) {
        //make the move
        board.move(m);
        //add # nodes in resulting position and depth - 1
        nodes += perft(board, depth - 1);
        //return to original state for next move in the position
        board.undo();
    }
    return nodes;
}
