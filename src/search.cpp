#include "search.h"

int ply = 0; //half move counter from root
static Move best_move;
static int nodes = 0;

int R = 2; //constant for null move pruning; Amount of depth reduction for null moves

//init pv length and pv table arrays
int pv_length[max_ply];
Move pv_table[max_ply][max_ply];

bool follow_pv;
bool score_pv;

const int full_depth_moves = 4;
const int reduction_limit = 3;

//enable PV move scoring
static void enable_pv_scoring(std::vector<Move>& moves) {
    //disable following pv

    follow_pv = false;
    //loop over move list
    for (int count = 0; count < moves.size(); ++count) {
        //make sure we hit PV move
        if (pv_table[0][ply] == moves[count]) {
            //enable move scoring
            score_pv = true;

            //enable following PV
            follow_pv = true;
        }
    }
}

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
    //define find PV node variable
    bool found_pv = false;

    //init pv length
    pv_length[ply] = ply;

    // Extend search by 1 if the side to move is in check
    if (board.in_check(board.curr_state().turn)) {
        depth++;
    }

    if (depth == 0) {
        //run quiescence search
        return quiescence(board, alpha, beta);
    }

    //we're too deep, so arrays using max_ply constants will overflow
    if (ply >= max_ply) {
        //return static eval of the current board
        return evaluate(board);
    }

    //increment nodes count
    nodes++;

    
        

    //no pruning: best move: b4c3, # nodes: 3176819
    //after pruning: best move: b4c3, # nodes: 1442900
    //null move pruning
    if (depth >= (R + 1) && !board.in_check(board.curr_state().turn) && ply) {
        //switch the side to give the opponent a free move
        board.switch_side();
        //reset en passant square
        uint8_t passant_sq = board.curr_state().passantTarget;
        board.curr_state().passantTarget = 0;
        
        //search move with reduced depth to find beta cutoffs
        int score = -negamax(board, -beta, -beta + 1, depth - 1 - R);

        //restore the position
        board.switch_side();
        board.curr_state().passantTarget = passant_sq;

        //fail hard beta cutoff
        if (score >= beta) {
            //node/move fails high
            return beta;
        }
    }

    //create movelist with only legal moves
    std::vector<Move> moves;
    board.generate_legal_moves(moves);

    //if following PV line
    if (follow_pv) {
        //enable PV move scoring
        enable_pv_scoring(moves);
    }

    // sort after PV flag is set
    sort_moves(board.curr_state(), moves);
    score_pv = false;

    bool in_check = board.in_check(board.curr_state().turn);

    //loop over moves in move list
    for (int count = 0; count < moves.size(); ++count) {
        //increment ply
        ply++;

        //make the move
        board.move(moves[count]);

        //declare score variable (from the static evaluation perspective)
        int score;

        //on PV node hit
        if (found_pv) {
            //condition to consider LMR (late move reduction)
            if (count >= full_depth_moves &&
                depth >= reduction_limit &&
                !in_check &&
                !board.in_check(board.curr_state().turn) &&
                !(moves[count].flags() & MF_CAPTURE) &&
                !(moves[count].promotion())) {
                //search current move with reduced depth
                score = -negamax(board, -alpha - 1, -alpha, depth - 2);

                //if reduced search improves alpha, re-search at normal depth
                if (score > alpha) {
                    score = -negamax(board, -alpha - 1, -alpha, depth - 1);
                }
            } else {
                //move between alpha and beta (PV move)
                score = -negamax(board, -alpha - 1, -alpha, depth - 1);
            }

            //a subsequent move was better than the PV move: searches in the normal alpha beta manner
            if ((score > alpha) && (score < beta)) {
                //re-search the move 
                score = -negamax(board, -beta, -alpha, depth - 1);
            }
        } else {
            //for all other types of moves, do normal alpha beta search
            score = -negamax(board, -beta, -alpha, depth - 1);
        }
        
        

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

            //enable found PV flag
            found_pv = true;

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
    //reset variables
    clear_vars();

    //iterative deepening
    for (int current_depth = 1; current_depth <= depth; ++current_depth) {
        //enable follow PV flag
        follow_pv = true;

        //find the best move within a given position
        int score = negamax(board, -50000, 50000, current_depth);

        for (int count = 0; count < pv_length[0]; ++count) {
            //print pv move
            std::cout << pv_table[0][count].to_string() << " ";
        }
        std::cout << std::endl;
    }

    std::cout << "best move: " << best_move.to_string() << std::endl;

    //placeholder
    return 1;
}

void search_and_print(ChessBoard& board, int depth) {
    int score = search_position(board, depth);
    std::cout << "score: " << score << ", nodes: " << nodes << std::endl;
}

Move find_best_move(ChessBoard& board, int depth) {
    search_position(board, depth);

    //for debugging/evaluation purposes
    print_nodes();

    return best_move;
}

void print_nodes() {
    std::cout << "# nodes: " << nodes << std::endl;
}

//clears variables for next search
static void clear_vars() {
    ply = 0;
    nodes = 0;
    best_move = Move();
    memset(killer_moves, 0, sizeof(killer_moves));
    memset(history_moves, 0, sizeof(history_moves));
    memset(pv_length, 0, sizeof(pv_length));
    memset(pv_table, 0, sizeof(pv_table));

    //reset follow pv flags
    follow_pv = false;
    score_pv = false;
}
