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
    constexpr int move_overhead_ms = 75;

    struct GoOptions {
        int depth = 0;
        int movetime = 0;
        int wtime = 0;
        int btime = 0;
        int winc = 0;
        int binc = 0;
        bool infinite = false;
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

    int allocate_time_ms(const ChessBoard& board, const GoOptions& options) {
        if (options.movetime > 0) {
            int time = options.movetime - move_overhead_ms;
            return time > 1 ? time : 1;
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
            return 0;
        }

        int budget = (time_left / 30) + (increment / 2);
        int max_budget = time_left / 4;
        if (budget > max_budget) {
            budget = max_budget;
        }

        budget -= move_overhead_ms;
        return budget > 1 ? budget : 1;
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
                    int budget = allocate_time_ms(*board, options);
                    set_search_time_limit(get_time_ms() + budget);
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
