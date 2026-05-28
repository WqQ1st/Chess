#include "uci.h"

#include "search.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include "board.h"

namespace {
    constexpr int default_search_depth = 5;

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

    int parse_go_depth(const std::string& line) {
        std::istringstream input(line);
        std::string token;
        int depth = default_search_depth;

        while (input >> token) {
            if (token == "depth" && input >> depth) {
                if (depth < 1) {
                    depth = 1;
                }
                return depth;
            }
        }

        return depth;
    }

    Move search_quietly(ChessBoard& board, int depth) {
        std::ostringstream debug_output;
        std::streambuf* old_cout = std::cout.rdbuf(debug_output.rdbuf());
        Move best = find_best_move(board, depth);
        std::cout.rdbuf(old_cout);
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
            int depth = parse_go_depth(line);
            std::vector<Move> legal_moves;
            board->generate_legal_moves(legal_moves);
            if (legal_moves.empty()) {
                std::cout << "bestmove 0000\n";
            } else {
                Move best = search_quietly(*board, depth);
                std::cout << "bestmove " << best.to_string() << "\n";
            }
        } else if (line == "stop") {
            std::cout << "bestmove 0000\n";
        } else if (line == "quit") {
            break;
        }
    }

    delete board;
}
