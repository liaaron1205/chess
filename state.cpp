#include "state.h"

#include <iostream>
#include <cstring>
#include <string>

using namespace std;

const string default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

State::State() : State::State(default_fen) {}

State::State(string fen) {
    uint64_t fen_idx = 0;

    // Initialize arrays to zero
    for (int i = 0; i<2; i++) {
        memset(board[i], 0, sizeof(board[i]));
        memset(castle[i], 0, sizeof(castle[i]));
    }

    // Process board
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
    fen_idx++;

    // Process the player to move
    if (fen[fen_idx]=='w') to_move = Player::White;
    else to_move = Player::Black;
    fen_idx += 2;

    // Process castling
    if (fen[fen_idx]=='-') fen_idx++;
    else{
        while(fen[fen_idx] != ' '){
            switch(fen[fen_idx]){
                case 'K': castle[(int)Player::White][(int)CastleSide::King  ] = 1;
                case 'Q': castle[(int)Player::White][(int)CastleSide::Queen ] = 1;
                case 'k': castle[(int)Player::Black][(int)CastleSide::King  ] = 1;
                case 'q': castle[(int)Player::Black][(int)CastleSide::Queen ] = 1;
            }
            fen_idx++;
        }
    }
    fen_idx++;

    // Process en-passant
    en_passant = "";
    while (fen[fen_idx] != ' '){
        en_passant += string(1, fen[fen_idx]);
        fen_idx++;
    }
    fen_idx++;

    string half_move_string = "", full_move_string = "";

    while (fen[fen_idx] != ' '){
        half_move_string += string(1, fen[fen_idx]);
        fen_idx++;
    }
    fen_idx++;

    while (fen_idx < fen.length() && fen[fen_idx] != ' '){
        full_move_string += string(1, fen[fen_idx]);
        fen_idx++;
    }
    fen_idx++;

    half_moves = stoi(half_move_string);
    full_moves = stoi(full_move_string);
}

void State::print(){
    for (char rank = '8'; rank >= '1' ; rank--){
        for (char file = 'a'; file <= 'h'; file++){
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
    cout << "K: " << castle[(int)Player::White][(int)CastleSide::King  ] << endl;
    cout << "Q: " << castle[(int)Player::White][(int)CastleSide::Queen ] << endl;
    cout << "k: " << castle[(int)Player::Black][(int)CastleSide::King  ] << endl;
    cout << "q: " << castle[(int)Player::Black][(int)CastleSide::Queen ] << endl;
    cout << en_passant << endl;
    cout << half_moves << endl;
    cout << full_moves << endl;
}