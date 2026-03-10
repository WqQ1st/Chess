#include "search.h"

static int ply = 0; //half move counter from root
static Move best_move;
static int nodes = 0;

//quiescence search
static int quiescence(ChessBoard& board, int alpha, int beta) {
    //evaluate position
    int eval = evaluate(board);
    //fail-hard beta cutoff
    if (eval >= beta) {
        //node (move) fails high
        return beta;
    }

    //found a better move
    if (eval > alpha) {
        //PV node (move)
        alpha = eval;
    }

    //create movelist with only legal moves
    std::vector<Move> moves;
    board.generate_legal_captures(moves);

    //loop over moves in move list
    for (int count = 0; count < moves.size(); ++count) {
        //increment ply
        ply++;

        //make the move
        board.move(moves[count]);

        //score current move
        int score = -quiescence(board, -beta, -alpha);

        //decrement ply
        ply--;

        //take back the move
        board.undo();

        //fail-hard beta cutoff
        if (score >= beta) {
            //node (move) fails high
            return beta;
        }

        //found a better move
        if (score > alpha) {
            //PV node (move)
            alpha = score;

            //update best move if root
            if (ply == 0) {
                best_move = moves[count];
            }
        }
    }

    //node (move) fails low
    return alpha;
}

//negamax alpha beta search
static int negamax(ChessBoard& board, int alpha, int beta, int depth) {
    if (depth == 0) {
        //run quiscence search
        return quiescence(board, alpha, beta);
    }

    //increment nodes count
    nodes++;

    //create movelist with only legal moves
    std::vector<Move> moves;
    board.generate_legal_moves(moves);

    //loop over moves in move list
    for (int count = 0; count < moves.size(); ++count) {
        //increment ply
        ply++;

        //make the move
        board.move(moves[count]);

        //score current move
        int score = -negamax(board, -beta, -alpha, depth - 1);

        //decrement ply
        ply--;

        //take back the move
        board.undo();

        //fail-hard beta cutoff
        if (score >= beta) {
            //node (move) fails high
            return beta;
        }

        //found a better move
        if (score > alpha) {
            //PV node (move)
            alpha = score;

            //update best move if root
            if (ply == 0) {
                best_move = moves[count];
            }
        }
    }

    //check for checkmate and stalemate (no legal moves)
    if (moves.size() == 0) {
        if (board.in_check(board.curr_state().turn)) {
            //return mating score
            return -49000 + ply;
        } else {
            //return stalemate score
            return 0;
        }
    }


    //node (move) fails low
    return alpha;
}

int search_position(ChessBoard& board, int depth) {
    ply = 0;
    nodes = 0;
    best_move = Move();

    //find the best move within a given position
    int score = negamax(board, -50000, 50000, depth);

    std::cout << "best move: " << best_move.to_string() << std::endl;

    return score;
}

Move find_best_move(ChessBoard& board, int depth) {
    ply = 0;
    nodes = 0;
    best_move = Move();

    negamax(board, -50000, 50000, depth);

    return best_move;
}
