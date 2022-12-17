#pragma once 

#include "utils.h"
#include <cstdint>
#include <string>

using namespace std;


class State{
    uint64_t board[2][NUM_PIECES];

    Color to_move;

    bool castle[2][2]; //0 = queen, 1 = king

    int8_t enpassant;

    uint_fast16_t half_moves;
    uint_fast16_t full_moves;

    public:
        State();
        State(string fen);

        void print_board();
};