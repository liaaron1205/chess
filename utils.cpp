#include "utils.h"
#include <iostream>

using namespace std;

uint8_t file_rank_to_idx(char file, char rank){
    uint8_t int_pos = (rank - '1') * 8 + (file - 'a');
    return int_pos;
}
string file_rank_to_move( char from_file, char from_rank, char to_file, char to_rank ){
    return string(1, from_file) + string(1, from_rank) + string(1, to_file) +  string(1, to_rank);
}

uint8_t str_to_idx(string pos){
    char file = get_file(pos);
    char rank = get_rank(pos);

    return file_rank_to_idx(file, rank);
}

uint64_t    bitset( uint8_t idx ){ uint64_t ret = 1ull << idx; return ret; }
bool        bitchk( uint64_t val, uint8_t idx ){ bool ret = val & bitset(idx); return ret; }

string idx_to_str(uint8_t idx){
    char file = get_file(idx);
    char rank = get_rank(idx);


    string s = string(1, file) + string(1, rank);
    return s;
}
string idx_to_move(uint8_t from, uint8_t to){
    return idx_to_str(from) + idx_to_str(to);
}

char get_file(string pos){ return pos[0]; }
char get_rank(string pos){ return pos[1]; }

char get_file( uint8_t idx ){ return idx % 8 + 'a'; }
char get_rank( uint8_t idx ){ return idx / 8 + '1'; }

Player other_player( Player p){ return (Player)(1 - (int)p); }