#include "search.h"

static int ply = 0; //half move counter from root
static Move best_move;
static int nodes = 0;

//init pv length and pv table arrays
int pv_length[64];
Move pv_table[64][64];

//quiescence search, returns eval 
static int quiescence(ChessBoard& board, int alpha, int beta) {
    //increment nodes count
    nodes++;

    //evaluate position
    int eval = evaluate(board);
    //fail-hard beta cutoff
    if (eval >= beta) {
        //node (move) fails high
        return beta;
    }

    //current position before moving > alpha, raise lower bound
    if (eval > alpha) {
        //PV node (move)
        alpha = eval;
    }

    //create movelist with only legal captures
    std::vector<Move> moves;
    board.generate_legal_captures(moves);

    //loop over moves in capture move list
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
    //init pv length
    pv_length[ply] = ply;

    if (depth == 0) {
        //run quiescence search
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
            //for only quiet moves
            if (!(moves[count].flags() & MF_CAPTURE)) {
                //store killer moves
                killer_moves[1][ply] = killer_moves[0][ply];
                killer_moves[0][ply] = moves[count];
            }

            //node (move) fails high
            return beta;
        }

        //found a better move
        if (score > alpha) {
            //for only quiet moves
            if (!(moves[count].flags() & MF_CAPTURE)) {
                //store history moves, gives slightly higher score for moves that were better than alpha
                history_moves[moves[count].piece()][moves[count].to()] += depth;
            }

            //PV node (move)
            alpha = score;

            //write PV move to table
            pv_table[ply][ply] = moves[count];

            //loop over the next ply, copy all moves from (ply + 1) line
            for (int next_ply = ply + 1; next_ply < pv_length[ply + 1]; ++next_ply) {
                //copy move from deeper ply into current line
                pv_table[ply][next_ply] = pv_table[ply + 1][next_ply];
            }

            //adjust pv length
            pv_length[ply] = pv_length[ply + 1];

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
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));

    //find the best move within a given position
    int score = negamax(board, -50000, 50000, depth);

    std::cout << "best move: " << best_move.to_string() << std::endl;

    return score;
}

void search_and_print(ChessBoard& board, int depth) {
    int score = search_position(board, depth);
    std::cout << "score: " << score << ", nodes: " << nodes << std::endl;
}

Move find_best_move(ChessBoard& board, int depth) {
    ply = 0;
    nodes = 0;
    best_move = Move();
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));

    negamax(board, -50000, 50000, depth);

    return best_move;
}

void print_nodes() {
    std::cout << "# nodes: " << nodes << std::endl;
}
