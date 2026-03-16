#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <cstdint>
#include "bitboards.h"
#include "attacks.h"
#include <math.h>
#include "board.h"
#include "square.h"
//board.h has "WHITE", "BLACK" enums 
#include <string.h>
#include <iostream>
#include <vector>

#include "constants.h"

using std::uint8_t;
using std::uint64_t;

enum class GenMode { All, CapturesOnly };

void generate_moves(const BoardState& state, std::vector<Move>& moves, GenMode mode);

static int score_move(ChessBoard& board, const Move& m);

static int sort_moves(ChessBoard& board, std::vector<Move>& moves);

//killer and history moves
//killer moves: [id][ply] id: 1st or 2nd move
extern Move killer_moves[2][max_ply];

//history moves: [piece][square]
extern int history_moves[12][64];

// most valuable victim & less valuable attacker

/*
                          
    (Victims) Pawn Knight Bishop   Rook  Queen   King
  (Attackers)
        Pawn   105    205    305    405    505    605
      Knight   104    204    304    404    504    604
      Bishop   103    203    303    403    503    603
        Rook   102    202    302    402    502    602
       Queen   101    201    301    401    501    601
        King   100    200    300    400    500    600

*/

// MVV LVA [attacker][victim]
const int mvv_lva[12][12] = {
 	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600,

	105, 205, 305, 405, 505, 605,  105, 205, 305, 405, 505, 605,
	104, 204, 304, 404, 504, 604,  104, 204, 304, 404, 504, 604,
	103, 203, 303, 403, 503, 603,  103, 203, 303, 403, 503, 603,
	102, 202, 302, 402, 502, 602,  102, 202, 302, 402, 502, 602,
	101, 201, 301, 401, 501, 601,  101, 201, 301, 401, 501, 601,
	100, 200, 300, 400, 500, 600,  100, 200, 300, 400, 500, 600
};


#endif
