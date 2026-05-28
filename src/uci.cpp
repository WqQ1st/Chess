#include "uci.h"

#include "enginetime.h"
#include "search.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "board.h"

namespace {
    constexpr int default_search_depth = 5;
    constexpr int max_timed_search_depth = 64;
    constexpr int move_overhead_ms = 100;

    struct GoOptions {
        int depth = 0;
        int movetime = 0;
        int wtime = 0;
        int btime = 0;
        int winc = 0;
        int binc = 0;
        int movestogo = 0;
        bool infinite = false;
    };

    struct TimeBudget {
        int soft_ms = 0;
        int hard_ms = 0;
    };

    bool make_uci_move(ChessBoard& board, const std::string& move_string) {
        std::vector<Move> legal_moves;
        board.generate_legal_moves(legal_moves);

        for (const Move& move : legal_moves) {
            if (move.to_string() == move_string) {
                board.move(move);
                return true;
            }
        }

        return false;
    }

    void parse_position(ChessBoard*& board, const std::string& line) {
        std::istringstream input(line);
        std::string token;

        input >> token; // position
        if (!(input >> token)) {
            return;
        }

        if (token == "startpos") {
            delete board;
            board = new ChessBoard(start_position);
            input >> token;
        } else if (token == "fen") {
            std::vector<std::string> fen_parts;
            while (input >> token && token != "moves") {
                fen_parts.push_back(token);
            }

            if (fen_parts.size() < 4) {
                return;
            }
            if (fen_parts.size() == 4) {
                fen_parts.push_back("0");
                fen_parts.push_back("1");
            } else if (fen_parts.size() == 5) {
                fen_parts.push_back("1");
            }

            std::string fen;
            for (std::size_t i = 0; i < fen_parts.size(); ++i) {
                if (i) {
                    fen += ' ';
                }
                fen += fen_parts[i];
            }

            delete board;
            board = new ChessBoard(fen.c_str());
        } else {
            return;
        }

        if (token != "moves") {
            return;
        }

        while (input >> token) {
            make_uci_move(*board, token);
        }
    }

    GoOptions parse_go_options(const std::string& line) {
        std::istringstream input(line);
        std::string token;
        GoOptions options;

        input >> token; // go

        while (input >> token) {
            if (token == "depth") {
                input >> options.depth;
            } else if (token == "movetime") {
                input >> options.movetime;
            } else if (token == "wtime") {
                input >> options.wtime;
            } else if (token == "btime") {
                input >> options.btime;
            } else if (token == "winc") {
                input >> options.winc;
            } else if (token == "binc") {
                input >> options.binc;
            } else if (token == "movestogo") {
                input >> options.movestogo;
            } else if (token == "infinite") {
                options.infinite = true;
            }
        }

        if (options.depth < 0) {
            options.depth = 0;
        }

        return options;
    }

    int search_depth_for_go(const GoOptions& options) {
        if (options.depth > 0) {
            return options.depth;
        }

        if (options.movetime > 0 || options.wtime > 0 || options.btime > 0) {
            return max_timed_search_depth;
        }

        return default_search_depth;
    }

    TimeBudget allocate_time_budget(const ChessBoard& board, const GoOptions& options) {
        if (options.movetime > 0) {
            int time = options.movetime - move_overhead_ms;
            if (time < 1) {
                time = 1;
            }
            return {time, time};
        }

        int time_left = 0;
        int increment = 0;
        if (board.curr_state().turn == WHITE) {
            time_left = options.wtime;
            increment = options.winc;
        } else {
            time_left = options.btime;
            increment = options.binc;
        }

        if (time_left <= 0) {
            return {0, 0};
        }

        int safe_time = time_left - move_overhead_ms;
        if (safe_time < 1) {
            return {1, 1};
        }

        int moves_to_go = options.movestogo > 0 ? options.movestogo : 18;
        int base = safe_time / moves_to_go;
        int soft = base + (increment * 3 / 4);

        int min_soft = increment > 0 ? increment / 2 : 0;
        if (soft < min_soft) {
            soft = min_soft;
        }

        int hard = soft * 3;
        int hard_cap = safe_time / 3;
        if (options.movestogo > 0) {
            hard_cap = safe_time / moves_to_go * 2;
        }

        if (hard > hard_cap) {
            hard = hard_cap;
        }
        if (hard < soft) {
            hard = soft;
        }
        if (hard > safe_time) {
            hard = safe_time;
        }
        if (soft > hard) {
            soft = hard;
        }

        return {soft > 1 ? soft : 1, hard > 1 ? hard : 1};
    }

    bool has_time_control(const GoOptions& options) {
        return options.movetime > 0 || options.wtime > 0 || options.btime > 0;
    }

    Move search_for_uci(ChessBoard& board, int depth) {
        set_uci_info_output(true);
        Move best = find_best_move(board, depth);
        set_uci_info_output(false);
        return best;
    }
}

void uci_loop() {
    std::string line;
    ChessBoard* board = new ChessBoard(start_position);

    while (std::getline(std::cin, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }

        if (line == "uci") {
            std::cout << "id name ChessEngine\n";
            std::cout << "id author WqQ\n";
            std::cout << "uciok\n";
        } else if (line == "isready") {
            std::cout << "readyok\n";
        } else if (line == "ucinewgame") {
            clear_transposition_table();
            delete board;
            board = new ChessBoard(start_position);
        } else if (line.rfind("position", 0) == 0) {
            parse_position(board, line);
        } else if (line.rfind("go", 0) == 0) {
            GoOptions options = parse_go_options(line);
            int depth = search_depth_for_go(options);
            std::vector<Move> legal_moves;
            board->generate_legal_moves(legal_moves);
            if (legal_moves.empty()) {
                std::cout << "bestmove 0000\n";
            } else {
                if (has_time_control(options)) {
                    TimeBudget budget = allocate_time_budget(*board, options);
                    uint64_t now = get_time_ms();
                    set_search_time_limit(now + budget.soft_ms, now + budget.hard_ms);
                } else {
                    clear_search_time_limit();
                }

                Move best = search_for_uci(*board, depth);
                clear_search_time_limit();
                std::cout << "bestmove " << best.to_string() << "\n";
            }
        } else if (line == "stop") {
            stop_search_now();
            std::cout << "bestmove 0000\n";
        } else if (line == "quit") {
            break;
        }
    }

    delete board;
}
