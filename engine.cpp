#include "engine.h"

#include <iostream>

using namespace std;

const int INF = 0x3f3f3f3f;

Engine::Engine(string s){
    if ( s == "white" ) engine_player = Player::White;
    else engine_player = Player::Black;
}

bool Engine::run(){
    string move;
    if (engine_player == current_state.get_to_move()){
        cout << "engine move" << endl;
        minimax(current_state, 35, -INF, INF, 1, 1);
        move = move_choice;
        cout << move << endl;
    }
    else {
        cout << "your move" << endl;
        cin >> move;
        if (move == "q") return 1;
    }
    current_state = State(current_state, move);

    
    return 0;
}

int Engine::minimax(State s, int depth, int alpha, int beta, bool maximize, bool top_level){  
    // cout << depth << endl;
    vector<string> all_moves, capture_moves;
    vector<State> all_states, capture_states;
    s.generate_moves(all_moves, capture_moves, all_states, capture_states);


    if (all_moves.size()==0){
        return -INF;
    }
    if ( s.get_half_moves() == 50 ){ return 0; }
    

    vector<string>& moves = all_moves;
    vector<State>&  states = all_states;
    /*if (capture_states.size()){
        moves = capture_moves;
        states = capture_states;
    }*/

    int depth_modifier = 10;
    if (states.size() == 1) depth_modifier = 0;
    else if (states.size() == 2) depth_modifier = 2;
    else if (states.size() == 3) depth_modifier = 4;
    else if (states.size() <= 5) depth_modifier = 5;

    if (depth_modifier != 0 && depth - depth_modifier < 0){
        return s.evaluate(engine_player);
    }

    if (maximize){
        int best = -INF;
        for (uint8_t i = 0; i<states.size(); i++){
            int candidate = minimax( states[i], depth - depth_modifier, alpha, beta, !maximize, 0);
            if ( candidate > best ){
                best = candidate;
                if (top_level) move_choice = moves[i];

                if ( candidate >= beta ) break;

                alpha = max(alpha, candidate);
            }
        }
        return best;
    }
    else{
        int best = INF;
        for (State s: states){
            int candidate = minimax( s, depth - depth_modifier, alpha, beta, !maximize, 0);
            best = min(best, candidate);

            if ( candidate <= alpha ) break;

            beta = min(beta, candidate);
        }
        return best;
    }
}