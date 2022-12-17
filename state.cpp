#include "state.h"

#include <iostream>
#include <cstring>

using namespace std;

const string default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

State::State() : State::State(default_fen) {}

State::State(string fen) {
    int fen_idx = 0;

    //Initialize bitboards to zero
    for (int i = 0; i<2; i++) {
        memset(board[i], 0, sizeof(board[i]));
        memset(castle[i], 0, castle(board[i]));
    }

    //Process board
    char file = 'a', rank = '8';
    while (fen[fen_idx] != ' '){
        char c = fen[fen_idx];
        if (c == '/') {
            rank -= 1;
            file = 'a';
        }
        else if (isdigit(c)) {
            file += c - '0';
        }
        else {
            uint8_t bit_idx = file_rank_to_index(file, rank);
            uint64_t or_value = ((uint64_t)1 << bit_idx);
            switch(c){
                case 'p': board[0][0] |= or_value;
                case 'n': board[0][1] |= or_value; 
                case 'b': board[0][2] |= or_value;
                case 'r': board[0][3] |= or_value;
                case 'q': board[0][4] |= or_value;
                case 'k': board[0][5] |= or_value;
                case 'P': board[1][0] |= or_value;
                case 'N': board[1][1] |= or_value;
                case 'B': board[1][2] |= or_value;
                case 'R': board[1][3] |= or_value;
                case 'Q': board[1][4] |= or_value;
                case 'K': board[1][5] |= or_value;
            }     
            file++;
        }
        fen_idx++;
    }

    //Process the player to move
    if (fen[fen_idx]=='w') to_move = white;
    else to_move = black;
    fen_idx += 2;

    if (fen[fen_idx]=='-'){

    }


    fen_idx++;

    //Process 

}

void State::print_board(){
    for (int rank = '8'; rank >= '1' ; rank--){
        for (int file = 'a'; file <= 'h'; file++){
            uint8_t bit_idx = file_rank_to_index(file, rank);
            uint64_t and_value = ((uint64_t)1 << bit_idx);
                 if (board[0][0] & and_value) cout << "p" ;
            else if (board[0][1] & and_value) cout << "n" ;
            else if (board[0][2] & and_value) cout << "b" ;
            else if (board[0][3] & and_value) cout << "r" ;
            else if (board[0][4] & and_value) cout << "q" ;
            else if (board[0][5] & and_value) cout << "k" ;
            else if (board[1][0] & and_value) cout << "P" ;
            else if (board[1][1] & and_value) cout << "N" ;
            else if (board[1][2] & and_value) cout << "B" ;
            else if (board[1][3] & and_value) cout << "R" ;
            else if (board[1][4] & and_value) cout << "Q" ;
            else if (board[1][5] & and_value) cout << "K" ;
            else cout << ".";
        }
        cout << endl;
    }
}