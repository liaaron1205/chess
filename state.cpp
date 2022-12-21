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
    full_moves = source.full_moves;

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
                    if (from == str_to_idx("a1")) castle[(int)Player::White][(int)CastleSide::Queen] = 0;
                    if (from == str_to_idx("h1")) castle[(int)Player::White][(int)CastleSide::King] = 0;
                }
                if (to_move == Player::Black){
                    if (from == str_to_idx("a8")) castle[(int)Player::Black][(int)CastleSide::Queen] = 0;
                    if (from == str_to_idx("h8")) castle[(int)Player::Black][(int)CastleSide::King] = 0;
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

            if ( i == (int)Piece::Rook){
                if (to_move == Player::White){
                    if (to   == str_to_idx("a8")) castle[(int)Player::Black][(int)CastleSide::Queen] = 0;
                    if (to   == str_to_idx("h8")) castle[(int)Player::Black][(int)CastleSide::King] = 0;
                }
                if (to_move == Player::Black){
                    if (to   == str_to_idx("a1")) castle[(int)Player::White][(int)CastleSide::Queen] = 0;
                    if (to   == str_to_idx("h1")) castle[(int)Player::White][(int)CastleSide::King] = 0;
                }
            }
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

string State::to_fen(){
    string ret = "";
    for ( char rank = '8'; rank>='1'; rank-- ){
        char empty = '0';
        for ( char file = 'a'; file<='h'; file++ ){
            uint8_t idx = file_rank_to_idx( file, rank );
            char update_char = '.';
            if ( bitchk( board[(int)Player::Black][(int)Piece::Pawn], idx ) )     update_char = 'p';
            if ( bitchk( board[(int)Player::Black][(int)Piece::Knight], idx ) )   update_char = 'n';
            if ( bitchk( board[(int)Player::Black][(int)Piece::Bishop], idx ) )   update_char = 'b';
            if ( bitchk( board[(int)Player::Black][(int)Piece::Rook], idx ) )     update_char = 'r';
            if ( bitchk( board[(int)Player::Black][(int)Piece::Queen], idx ) )    update_char = 'q';
            if ( bitchk( board[(int)Player::Black][(int)Piece::King], idx ) )     update_char = 'k';
            if ( bitchk( board[(int)Player::White][(int)Piece::Pawn], idx ) )     update_char = 'P';
            if ( bitchk( board[(int)Player::White][(int)Piece::Knight], idx ) )   update_char = 'N';
            if ( bitchk( board[(int)Player::White][(int)Piece::Bishop], idx ) )   update_char = 'B';
            if ( bitchk( board[(int)Player::White][(int)Piece::Rook], idx ) )     update_char = 'R';
            if ( bitchk( board[(int)Player::White][(int)Piece::Queen], idx ) )    update_char = 'Q';
            if ( bitchk( board[(int)Player::White][(int)Piece::King], idx ) )     update_char = 'K';

            if ( update_char == '.' ) empty++;
            else {
                if (empty != '0'){
                    ret += string(1, empty);
                    empty = '0';
                }
                ret += update_char;
            }
        }
        if ( empty != '0' ) ret += string(1, empty);
        if (rank != '1') ret += "/";
    }
    ret += " ";
    ret += (to_move == Player::White) ? "w" : "b";
    ret += " ";
    bool any_castle = 0;
    if ( castle[(int)Player::White][(int)CastleSide::King]) {
        any_castle = 1;
        ret += "K";
    }
    if ( castle[(int)Player::White][(int)CastleSide::Queen]) {
        any_castle = 1;
        ret += "Q";
    }
    if ( castle[(int)Player::Black][(int)CastleSide::King]) {
        any_castle = 1;
        ret += "k";
    }
    if ( castle[(int)Player::Black][(int)CastleSide::Queen]) {
        any_castle = 1;
        ret += "q";
    }
    if ( !any_castle ) ret += "-";

    ret += " ";
    ret += (en_passant == 64) ? "-" : idx_to_str(en_passant);
    ret += " ";
    ret += to_string(half_moves);
    ret += " ";
    ret += to_string(full_moves);

    return ret;
}

bool State::is_check(bool invert){ //0: are you in check? 1: is the opponent in check?
    Player p = to_move;
    if (invert) p = other_player(p);

    Player opp = other_player(p);

    uint64_t allboards = 0ll;
    for ( int i=0; i<2; i++ ) for ( int j = 0; j<NUM_PIECES; j++ ) allboards |= board[i][j];

    for (int i = 0; i<64; i++){
        if (bitchk(board[(int)p][(int)Piece::King], i)){
            char rank = get_rank(i);
            char file = get_file(i);
            

            // Pawns
            if (p == Player::White) {
                if (file-1 >= 'a' && bitchk( board[(int)Player::Black][(int)Piece::Pawn], file_rank_to_idx( file - 1, rank + 1 ))) return 1;
                if (file+1 <= 'h' && bitchk( board[(int)Player::Black][(int)Piece::Pawn], file_rank_to_idx( file + 1, rank + 1 ))) return 1;
            }
            if (p == Player::Black) {
                if (file-1 >= 'a' && bitchk( board[(int)Player::White][(int)Piece::Pawn], file_rank_to_idx( file - 1, rank - 1 ))) return 1;
                if (file+1 <= 'h' && bitchk( board[(int)Player::White][(int)Piece::Pawn], file_rank_to_idx( file + 1, rank - 1 ))) return 1;
            }

            // Knights
            if (file-1 >= 'a' && rank-2 >= '1' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file - 1, rank - 2 ))) return 1;
            if (file-1 >= 'a' && rank+2 <= '8' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file - 1, rank + 2 ))) return 1;
            if (file+1 <= 'h' && rank-2 >= '1' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file + 1, rank - 2 ))) return 1;
            if (file+1 <= 'h' && rank+2 <= '8' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file + 1, rank + 2 ))) return 1;
            if (file-2 >= 'a' && rank-1 >= '1' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file - 2, rank - 1 ))) return 1;
            if (file-2 >= 'a' && rank+1 <= '8' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file - 2, rank + 1 ))) return 1;
            if (file+2 <= 'h' && rank-1 >= '1' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file + 2, rank - 1 ))) return 1;
            if (file+2 <= 'h' && rank+1 <= '8' && bitchk( board[(int)opp][(int)Piece::Knight], file_rank_to_idx( file + 2, rank + 1 ))) return 1;

             // King
            if (file-1 >= 'a' && rank-1 >= '1' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file - 1, rank - 1 ))) return 1;
            if (file-1 >= 'a' && rank+1 <= '8' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file - 1, rank + 1 ))) return 1;
            if (file+1 <= 'h' && rank-1 >= '1' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file + 1, rank - 1 ))) return 1;
            if (file+1 <= 'h' && rank+1 <= '8' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file + 1, rank + 1 ))) return 1;
            if (file-1 >= 'a' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file - 1, rank ))) return 1;
            if (rank-1 >= '1' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file, rank - 1 ))) return 1;
            if (file+1 <= 'h' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file + 1, rank ))) return 1;
            if (rank+1 <= '8' && bitchk( board[(int)opp][(int)Piece::King], file_rank_to_idx( file, rank + 1 ))) return 1;

            // NE Diagonal
            for (uint8_t k = 1; file+k<='h' && rank+k<='8'; k++){
                uint64_t idx = file_rank_to_idx(file + k, rank + k);
                if (bitchk(board[(int)opp][(int)Piece::Bishop], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // SE Diagonal
            for (uint8_t k = 1; file+k<='h' && rank-k>='1'; k++){
                uint64_t idx = file_rank_to_idx(file + k, rank - k);
                if (bitchk(board[(int)opp][(int)Piece::Bishop], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // NW Diagonal
            for (uint8_t k = 1; file-k>='a' && rank+k<='8'; k++){
                uint64_t idx = file_rank_to_idx(file - k, rank + k);
                if (bitchk(board[(int)opp][(int)Piece::Bishop], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // SW Diagonal
            for (uint8_t k = 1; file-k>='a' && rank-k>='1'; k++){
                uint64_t idx = file_rank_to_idx(file - k, rank - k);
                if (bitchk(board[(int)opp][(int)Piece::Bishop], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // East
            for (uint8_t k = 1; file+k<='h'; k++){
                uint64_t idx = file_rank_to_idx(file + k, rank);
                if (bitchk(board[(int)opp][(int)Piece::Rook], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // North
            for (uint8_t k = 1; rank+k<='8'; k++){
                uint64_t idx = file_rank_to_idx(file, rank + k);
                if (bitchk(board[(int)opp][(int)Piece::Rook], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // West
            for (uint8_t k = 1; file-k>='a'; k++){
                uint64_t idx = file_rank_to_idx(file - k, rank);
                if (bitchk(board[(int)opp][(int)Piece::Rook], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            // South
            for (uint8_t k = 1; rank-k>='1'; k++){
                uint64_t idx = file_rank_to_idx(file, rank - k);
                if (bitchk(board[(int)opp][(int)Piece::Rook], idx) || bitchk(board[(int)opp][(int)Piece::Queen], idx)) return 1;
                else if (bitchk(allboards, idx)) break;
            }
            
        }
    }
    return 0;
}

void State::process_move (string move, vector<string>& all_moves, vector<string>& take_moves, vector<State>& all_states, vector<State>& take_states){
    State result = State(*this, move);
    bool capture = result.pieces != pieces;
    

    if (!result.is_check(1)){
        if (move == "e1c1" && to_move == Player::White && castle[(int)Player::White][(int)CastleSide::Queen]){
            State tmp_result = State(*this, "e1d1");
            if ( is_check() || tmp_result.is_check(1)) return;
        }
        if (move == "e1g1" && to_move == Player::White && castle[(int)Player::White][(int)CastleSide::King]) {
             State tmp_result = State(*this, "e1f1");
            if ( is_check() || tmp_result.is_check(1)) return;
        }
        if (move == "e8c8" && to_move == Player::Black && castle[(int)Player::Black][(int)CastleSide::Queen]){
            State tmp_result = State(*this, "e8d8");
            if ( is_check() || tmp_result.is_check(1)) return;
        }
        if (move == "e8g8" && to_move == Player::Black && castle[(int)Player::Black][(int)CastleSide::King]) {
            State tmp_result = State(*this, "e8f8");
            if ( is_check() || tmp_result.is_check(1)) return;
        }

        all_moves.push_back(move);
        all_states.push_back(result);
        if (capture){
            take_moves.push_back(move);
            take_states.push_back(result);
        }
    }
}


void State::generate_moves(vector<string>& all_moves, vector<string>& take_moves, vector<State>& all_states, vector<State>& take_states){
    bool you = (bool)to_move;
    bool opp = 1 - (bool)to_move;

    int forward = you? 1 : -1;
    int backward = -1 * forward;

    char close_rank = '8' - 7 * you;
    char far_rank = '1' + 7 * you;

    uint64_t boards[2] = {0ll, 0ll};
    for ( int i=0; i<2; i++ ) for ( int j = 0; j<NUM_PIECES; j++ ) boards[i] |= board[i][j];
    uint64_t allboards = boards[0] | boards[1];


    for ( uint8_t i = 0; i < 64; i++ ){
        char file = get_file(i), rank = get_rank(i);
        if ( bitchk(board[you][(int)Piece::Pawn], i) && rank != far_rank){
            vector<string> promotion_suffix = {""};
            if (rank == far_rank + backward) promotion_suffix = {"n", "b", "r", "q"};
            if (!bitchk(allboards, file_rank_to_idx(file, rank+forward))) for (string suffix : promotion_suffix) process_move( file_rank_to_move(file, rank, file, rank+forward) + suffix, all_moves, take_moves, all_states, take_states );

            if ( file - 1 >= 'a' && (bitchk(boards[opp], file_rank_to_idx( file - 1, rank + forward)) || en_passant == file_rank_to_idx( file - 1, rank + forward ))) for (string suffix : promotion_suffix) process_move( file_rank_to_move(file, rank, file-1, rank+forward) + suffix, all_moves, take_moves, all_states, take_states );
            if ( file + 1 <= 'h' && (bitchk(boards[opp], file_rank_to_idx( file + 1, rank + forward)) || en_passant == file_rank_to_idx( file + 1, rank + forward ))) for (string suffix : promotion_suffix) process_move( file_rank_to_move(file, rank, file+1, rank+forward) + suffix, all_moves, take_moves, all_states, take_states );

            if (rank == close_rank + forward && !bitchk(allboards, file_rank_to_idx(file, rank+forward)) && !bitchk(allboards, file_rank_to_idx(file, rank+2*forward))) process_move( file_rank_to_move(file, rank, file, rank+2*forward), all_moves, take_moves, all_states, take_states );
        }
        if ( bitchk(board[you][(int)Piece::Knight], i) ){
            if (file-1 >= 'a' && rank-2 >= '1' && !bitchk( boards[you], file_rank_to_idx( file - 1, rank - 2 ))) process_move( file_rank_to_move( file, rank, file - 1, rank - 2 ), all_moves, take_moves, all_states, take_states);
            if (file-1 >= 'a' && rank+2 <= '8' && !bitchk( boards[you], file_rank_to_idx( file - 1, rank + 2 ))) process_move( file_rank_to_move( file, rank, file - 1, rank + 2 ), all_moves, take_moves, all_states, take_states);
            if (file+1 <= 'h' && rank-2 >= '1' && !bitchk( boards[you], file_rank_to_idx( file + 1, rank - 2 ))) process_move( file_rank_to_move( file, rank, file + 1, rank - 2 ), all_moves, take_moves, all_states, take_states);
            if (file+1 <= 'h' && rank+2 <= '8' && !bitchk( boards[you], file_rank_to_idx( file + 1, rank + 2 ))) process_move( file_rank_to_move( file, rank, file + 1, rank + 2 ), all_moves, take_moves, all_states, take_states);
            if (file-2 >= 'a' && rank-1 >= '1' && !bitchk( boards[you], file_rank_to_idx( file - 2, rank - 1 ))) process_move( file_rank_to_move( file, rank, file - 2, rank - 1 ), all_moves, take_moves, all_states, take_states);
            if (file-2 >= 'a' && rank+1 <= '8' && !bitchk( boards[you], file_rank_to_idx( file - 2, rank + 1 ))) process_move( file_rank_to_move( file, rank, file - 2, rank + 1 ), all_moves, take_moves, all_states, take_states);
            if (file+2 <= 'h' && rank-1 >= '1' && !bitchk( boards[you], file_rank_to_idx( file + 2, rank - 1 ))) process_move( file_rank_to_move( file, rank, file + 2, rank - 1 ), all_moves, take_moves, all_states, take_states);
            if (file+2 <= 'h' && rank+1 <= '8' && !bitchk( boards[you], file_rank_to_idx( file + 2, rank + 1 ))) process_move( file_rank_to_move( file, rank, file + 2, rank + 1 ), all_moves, take_moves, all_states, take_states);
        }
        if ( bitchk(board[you][(int)Piece::King], i) ) {
            if (file-1 >= 'a' && rank-1 >= '1' && !bitchk( boards[you], file_rank_to_idx( file - 1, rank - 1 ))) process_move( file_rank_to_move( file, rank, file - 1, rank - 1 ), all_moves, take_moves, all_states, take_states);
            if (file-1 >= 'a' && rank+1 <= '8' && !bitchk( boards[you], file_rank_to_idx( file - 1, rank + 1 ))) process_move( file_rank_to_move( file, rank, file - 1, rank + 1 ), all_moves, take_moves, all_states, take_states);
            if (file+1 <= 'h' && rank-1 >= '1' && !bitchk( boards[you], file_rank_to_idx( file + 1, rank - 1 ))) process_move( file_rank_to_move( file, rank, file + 1, rank - 1 ), all_moves, take_moves, all_states, take_states);
            if (file+1 <= 'h' && rank+1 <= '8' && !bitchk( boards[you], file_rank_to_idx( file + 1, rank + 1 ))) process_move( file_rank_to_move( file, rank, file + 1, rank + 1 ), all_moves, take_moves, all_states, take_states);
            if (file-1 >= 'a' && !bitchk( boards[you], file_rank_to_idx( file - 1, rank ))) process_move( file_rank_to_move( file, rank, file - 1, rank ), all_moves, take_moves, all_states, take_states);
            if (rank-1 >= '1' && !bitchk( boards[you], file_rank_to_idx( file, rank - 1 ))) process_move( file_rank_to_move( file, rank, file, rank - 1 ), all_moves, take_moves, all_states, take_states);
            if (file+1 <= 'h' && !bitchk( boards[you], file_rank_to_idx( file + 1, rank ))) process_move( file_rank_to_move( file, rank, file + 1, rank ), all_moves, take_moves, all_states, take_states);
            if (rank+1 <= '8' && !bitchk( boards[you], file_rank_to_idx( file, rank + 1 ))) process_move( file_rank_to_move( file, rank, file, rank + 1 ), all_moves, take_moves, all_states, take_states);

            if (to_move == Player::White){
                if ( castle[you][(int)CastleSide::Queen] && !bitchk( allboards, file_rank_to_idx('b', '1')) && !bitchk( allboards, file_rank_to_idx('c', '1')) && !bitchk( allboards, file_rank_to_idx('d', '1')))  process_move( "e1c1", all_moves, take_moves, all_states, take_states);
                if ( castle[you][(int)CastleSide::King] && !bitchk( allboards, file_rank_to_idx('f', '1')) && !bitchk( allboards, file_rank_to_idx('g', '1')))  process_move( "e1g1", all_moves, take_moves, all_states, take_states);
            }
            if (to_move == Player::Black){
                if ( castle[you][(int)CastleSide::Queen] && !bitchk( allboards, file_rank_to_idx('b', '8')) && !bitchk( allboards, file_rank_to_idx('c', '8')) && !bitchk( allboards, file_rank_to_idx('d', '8')))  process_move( "e8c8", all_moves, take_moves, all_states, take_states);
                if ( castle[you][(int)CastleSide::King] && !bitchk( allboards, file_rank_to_idx('f', '8')) && !bitchk( allboards, file_rank_to_idx('g', '8')))  process_move( "e8g8", all_moves, take_moves, all_states, take_states);
            }    
        }
        if ( bitchk(board[you][(int)Piece::Bishop], i) || bitchk(board[you][(int)Piece::Queen], i) ){
            // NE Diagonal
            for (uint8_t k = 1; file+k<='h' && rank+k<='8'; k++){
                uint64_t idx = file_rank_to_idx(file + k, rank + k);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file + k, rank + k ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file + k, rank + k ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
            // SE Diagonal
            for (uint8_t k = 1; file+k<='h' && rank-k>='1'; k++){
                uint64_t idx = file_rank_to_idx(file + k, rank - k);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file + k, rank - k ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file + k, rank - k ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
            // NW Diagonal
            for (uint8_t k = 1; file-k>='a' && rank+k<='8'; k++){
                uint64_t idx = file_rank_to_idx(file - k, rank + k);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file - k, rank + k ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file - k, rank + k ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
            // SW Diagonal
            for (uint8_t k = 1; file-k>='a' && rank-k>='1'; k++){
                uint64_t idx = file_rank_to_idx(file - k, rank - k);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file - k, rank - k ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file - k, rank - k ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
        }
        if ( bitchk(board[you][(int)Piece::Rook], i) || bitchk(board[you][(int)Piece::Queen], i) ){
            // East
            for (uint8_t k = 1; file+k<='h'; k++){
                uint64_t idx = file_rank_to_idx(file + k, rank);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file + k, rank ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file + k, rank  ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
            // North
            for (uint8_t k = 1; rank+k<='8'; k++){
                uint64_t idx = file_rank_to_idx(file, rank + k);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file, rank + k ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file, rank + k ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
            // West
            for (uint8_t k = 1; file-k>='a'; k++){
                uint64_t idx = file_rank_to_idx(file - k, rank);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file - k, rank ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file - k, rank ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
            // South
            for (uint8_t k = 1; rank-k>='1'; k++){
                uint64_t idx = file_rank_to_idx(file, rank - k);
                if ( !bitchk( allboards, idx) ) process_move( file_rank_to_move( file, rank, file, rank - k ), all_moves, take_moves, all_states, take_states );
                else {
                    if ( bitchk(boards[opp], idx) ) process_move( file_rank_to_move( file, rank, file, rank - k ), all_moves, take_moves, all_states, take_states );
                    break;
                }
            }
        }
    }
}

uint_fast16_t State::get_half_moves(){ return half_moves; }
Player State::get_to_move(){ return to_move; }

int State::evaluate (Player p){
    int points[NUM_PIECES] = {1, 3, 3, 5, 9, 0};
    int piece_weight = 30;

    int heuristic = 0;
    int total_pieces = 0;
    for (int i = 0; i<NUM_PIECES; i++){
        heuristic += points[i] * __builtin_popcountll(board[(int)p][i]) * piece_weight;
        heuristic -= points[i] * __builtin_popcountll(board[1 - (int)p][i]) * piece_weight;

        total_pieces += __builtin_popcountll(board[(int)p][i]) + __builtin_popcountll(board[1 - (int)p][i]);


        const uint64_t mask1 = 0b0000000000000000000000000001100000011000000000000000000000000000;
        const uint64_t mask2 = 0b0000000000000000001111000010010000100100001111000000000000000000;
        const uint64_t mask3 = 0b0000000000000000110000111100001111000011110000110000000000000000;

        if ( i != (int) Piece::King){
            heuristic += 4 * __builtin_popcountll(board[(int)p][i] & mask1);
            heuristic += 2 * __builtin_popcountll(board[(int)p][i] & mask2);
            heuristic += 1 * __builtin_popcountll(board[(int)p][i] & mask3);
        }
    }

    if ( castle[(int)p][(int)CastleSide::King] || castle[(int)p][(int)CastleSide::Queen] ) heuristic += 10;

    if (total_pieces < 5){
        for (int i = 0; i<64; i++){
            
            if (bitchk(board[1 - (int)p][(int)Piece::King], i)){
                int x = 3 * ( max(get_file(i) - 'a', 'h' - get_file(i)) + max(get_rank(i) - '1', '8' - get_rank(i))  - 8);
                heuristic += x;
            }
        }
    }

    return heuristic;
}   



