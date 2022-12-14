#pragma once 

#include "utils.h"

#include <cstdint>
#include <string>
#include <vector>

using namespace std;


class State{
    uint64_t board[2][NUM_PIECES];

    Player to_move;

    bool castle[2][2]; //0 = queen, 1 = king

    uint8_t en_passant;

    uint_fast16_t half_moves;
    uint_fast16_t full_moves;

    uint8_t pieces;

    public:
        State();
        State(string fen);
        State(const State& source, string move); //Create state by applying move to another state

        void print();
        string to_fen();

        bool is_check(bool invert = false); //0: are you in check? 1: is the opponent in check?

        void process_move (string move, vector<string>& all_moves, vector<string>& take_moves, vector<State>& all_states, vector<State>& take_states);
        void generate_moves(vector<string>& all_moves, vector<string>& take_moves, vector<State>& all_states, vector<State>& take_states);

        Player get_to_move();
        uint_fast16_t get_half_moves();

        int evaluate (Player p);

};