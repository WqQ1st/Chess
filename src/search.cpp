#include "search.h"

static int ply = 0; //half move counter from root
static Move best_move;
static int nodes = 0;

//negamax alpha beta search
static int negamax(ChessBoard& board, int alpha, int beta, int depth) {
    if (depth == 0) {
        //return evaluation
        return evaluate(board);
    }

    //increment nodes count
    nodes++;

    //create movelist
    std::vector<Move> moves;
    generate_moves(board.curr_state(), moves, GenMode::All);

    return 0;
}

int search_position(ChessBoard& board, int depth) {
    

    return 0;
}
