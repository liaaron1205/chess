#pragma once 

#include "state.h"

class Engine{
    State current_state;
    Player engine_player;
    string move_choice;


    public:
        Engine(string s);

        bool run();

    private:
        int minimax(State s, int depth, int alpha, int beta, bool maximize, bool top_level);
    
};