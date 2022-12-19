#include "state.h"

#include <iostream>
#include <cstring>
#include <string>
#include <map>

using namespace std;

const string default_fen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

State::State() : State::State(default_fen) {}

State::State(string fen) {
    uint64_t fen_idx = 0;
    
    pieces = 0;

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
            uint8_t bit_idx = file_rank_to_idx(file, rank);
            uint64_t or_value = (1ull << bit_idx);
            if ( c == 'p')  board[(int)Player::Black][(int)Piece::Pawn]     |= or_value;
            if ( c == 'n')  board[(int)Player::Black][(int)Piece::Knight]   |= or_value; 
            if ( c == 'b')  board[(int)Player::Black][(int)Piece::Bishop]   |= or_value;
            if ( c == 'r')  board[(int)Player::Black][(int)Piece::Rook]     |= or_value;
            if ( c == 'q')  board[(int)Player::Black][(int)Piece::Queen]    |= or_value;
            if ( c == 'k')  board[(int)Player::Black][(int)Piece::King]     |= or_value;
            if ( c == 'P')  board[(int)Player::White][(int)Piece::Pawn]     |= or_value;
            if ( c == 'N')  board[(int)Player::White][(int)Piece::Knight]   |= or_value; 
            if ( c == 'B')  board[(int)Player::White][(int)Piece::Bishop]   |= or_value;
            if ( c == 'R')  board[(int)Player::White][(int)Piece::Rook]     |= or_value;
            if ( c == 'Q')  board[(int)Player::White][(int)Piece::Queen]    |= or_value;
            if ( c == 'K')  board[(int)Player::White][(int)Piece::King]     |= or_value;   
            file++;
            pieces++;
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
                case 'K':
                    castle[(int)Player::White][(int)CastleSide::King  ] = 1;
                    break;
                case 'Q':
                    castle[(int)Player::White][(int)CastleSide::Queen ] = 1;
                    break;
                case 'k':
                    castle[(int)Player::Black][(int)CastleSide::King  ] = 1;
                    break;
                case 'q':
                    castle[(int)Player::Black][(int)CastleSide::Queen ] = 1;
                    break;
            }
            fen_idx++;
        }
    }
    fen_idx++;

    // Process en-passant
    en_passant = 64;
    string en_passant_str = "";
    while (fen[fen_idx] != ' '){
        en_passant_str += string(1, fen[fen_idx]);
        fen_idx++;
    }
    if (en_passant_str != "-") en_passant = str_to_idx(en_passant_str);
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

State::State(const State& source, string move){
    for (int i = 0; i<2; i++) for (int j = 0; j<NUM_PIECES; j++) board[i][j] = source.board[i][j];
    to_move = source.to_move;
    for (int i = 0; i<2; i++) for (int j = 0; j<2; j++) castle[i][j] = source.castle[i][j];

    en_passant = 64; 
    half_moves = source.half_moves + 1;
    full_moves = source.full_moves + 1;

    pieces = source.pieces;

    uint8_t from = str_to_idx(move.substr(0, 2));
    uint8_t to = str_to_idx(move.substr(2, 2));
    string suffix = move.substr(4);

    for (int i = 0; i<NUM_PIECES; i++){
        if (bitchk(board[(int)to_move][i], from)){
            board[(int)to_move][i] &= ~(bitset(from));
            board[(int)to_move][i] |= bitset(to);
            if (i == (int)Piece::Pawn){
                half_moves = 0;
                if (abs(get_rank(from) - get_rank(to))>1) en_passant = (from + to)/2;
                if (to == source.en_passant){
                    if (to_move == Player::White) board[(int)other_player(to_move)][i] &= ~(bitset(to - 8));
                    if (to_move == Player::Black) board[(int)other_player(to_move)][i] &= ~(bitset(to + 8));
                } 
            }
            if (i == (int)Piece::King){
                castle[(int)to_move][0] = castle[(int)to_move][1] = 0;
            }
            if (i == (int)Piece::Rook){
                if (to_move == Player::White){
                    if (from == str_to_idx("a1")) castle[(int)to_move][(int)CastleSide::Queen] = 0;
                    if (from == str_to_idx("h1")) castle[(int)to_move][(int)CastleSide::King] = 0;
                }
                if (to_move == Player::Black){
                    if (from == str_to_idx("a8")) castle[(int)to_move][(int)CastleSide::Queen] = 0;
                    if (from == str_to_idx("h8")) castle[(int)to_move][(int)CastleSide::King] = 0;
                }
            }
            break;
        }
    }
    for (int i = 0; i<NUM_PIECES; i++){
        if (bitchk(board[(int)other_player(to_move)][i], to)){
            board[(int)other_player(to_move)][i] &= ~(bitset(to));
            pieces--;
            half_moves = 0;
        }
    }
    map<string, Piece> str_to_piece = {
        {"n", Piece::Knight},
        {"b", Piece::Bishop},
        {"r", Piece::Rook},
        {"q", Piece::Queen}
    };

    if (suffix != "") {
        board[(int)to_move][(int)Piece::Pawn] &= ~(bitset(to));
        board[(int)to_move][(int)str_to_piece[suffix]] |= bitset(to);
    }

    if (move == "e1c1" && to_move == Player::White && source.castle[(int)Player::White][(int)CastleSide::Queen]){
        board[(int)to_move][(int)Piece::Rook] &= ~(bitset(str_to_idx("a1")));
        board[(int)to_move][(int)Piece::Rook] |= bitset(str_to_idx("d1"));
    }
    if (move == "e1g1" && to_move == Player::White && source.castle[(int)Player::White][(int)CastleSide::King]) {
        board[(int)to_move][(int)Piece::Rook] &= ~(bitset(str_to_idx("h1")));
        board[(int)to_move][(int)Piece::Rook] |= bitset(str_to_idx("f1"));
    }
    if (move == "e8c8" && to_move == Player::Black && source.castle[(int)Player::Black][(int)CastleSide::Queen]){
        board[(int)to_move][(int)Piece::Rook] &= ~(bitset(str_to_idx("a8")));
        board[(int)to_move][(int)Piece::Rook] |= bitset(str_to_idx("d8"));
    }
    if (move == "e8g8" && to_move == Player::Black && source.castle[(int)Player::Black][(int)CastleSide::King]) {
        board[(int)to_move][(int)Piece::Rook] &= ~(bitset(str_to_idx("h8")));
        board[(int)to_move][(int)Piece::Rook] |= bitset(str_to_idx("f8"));
    }

    if (to_move == Player::Black) full_moves++;
    to_move = other_player(to_move);
}

void State::print(){
    // cout << " abcdefgh " << endl;
    for (char rank = '8'; rank >= '1' ; rank--){
        // cout << rank;
        for (char file = 'a'; file <= 'h'; file++){
            uint8_t bit_idx = file_rank_to_idx(file, rank);
            uint64_t and_value = ((uint64_t)1 << bit_idx);
                    if (board[0][0] & and_value) cout << "p" ;
            else    if (board[0][1] & and_value) cout << "n" ;
            else    if (board[0][2] & and_value) cout << "b" ;
            else    if (board[0][3] & and_value) cout << "r" ;
            else    if (board[0][4] & and_value) cout << "q" ;
            else    if (board[0][5] & and_value) cout << "k" ;
            else    if (board[1][0] & and_value) cout << "P" ;
            else    if (board[1][1] & and_value) cout << "N" ;
            else    if (board[1][2] & and_value) cout << "B" ;
            else    if (board[1][3] & and_value) cout << "R" ;
            else    if (board[1][4] & and_value) cout << "Q" ;
            else    if (board[1][5] & and_value) cout << "K" ;
            else        cout << ".";
        }
        //cout << rank << endl;
        cout << endl;
    }
    cout << (int)pieces << endl;
    cout << "K: " << castle[(int)Player::White][(int)CastleSide::King  ] << endl;
    cout << "Q: " << castle[(int)Player::White][(int)CastleSide::Queen ] << endl;
    cout << "k: " << castle[(int)Player::Black][(int)CastleSide::King  ] << endl;
    cout << "q: " << castle[(int)Player::Black][(int)CastleSide::Queen ] << endl;
    cout << ((en_passant == 64) ? "-" : idx_to_str(en_passant)) << endl;
    cout << half_moves << endl;
    cout << full_moves << endl;
}

void State::generate_moves(vector<string>& all_moves, vector<string>& take_moves){
    bool you = (bool)to_move;
    bool opp = 1 - (bool)to_move;

    int forward = you? 1 : -1;
    int backward = -1 * forward;

    char close_rank = '8' - 7 * you;
    char far_rank = '1' + 7 * you;

    uint64_t boards[2] = {0ll, 0ll};
    for ( int i=0; i<2; i++ ) for ( int j = 0; j<NUM_PIECES; j++ ) boards[i] |= board[i][j];

    // Pawns
    uint64_t pawn_board = board[you][(int)Piece::Pawn];
    for ( uint8_t i = 0; i < 64; i++ ){
        if ( pawn_board & (1ull << i) ){
            // Default
            if (get_rank(i) != far_rank) {
                vector<string> promotion_suffix;
                if (get_rank(i) == far_rank + backward * i) promotion_suffix = {"k", "b", "r", "q"};
                if (!((boards[0] | boards[1]) & bitset(i + forward * 8))) all_moves.push_back( idx_to_move(i, i + forward*8) );

                // Left 
                if ((get_file(i) > 'a') && (boards[opp] & bitset(i + forward * 8 - 1))) {
                    all_moves.push_back( idx_to_move( i, i + forward*8 - 1 ));
                    take_moves.push_back( idx_to_move( i, i + forward*8 - 1 ));
                }
                // Right
                if ((get_file(i) < 'h') && (boards[opp] & bitset(i + forward * 8 + 1))) {
                    all_moves.push_back( idx_to_move( i, i + forward*8 + 1 ));
                    take_moves.push_back( idx_to_move( i, i + forward*8 + 1 ));
                }
            }
        }
    }  
    
}
