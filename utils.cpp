#include "utils.h"
#include <iostream>

using namespace std;

uint8_t file_rank_to_index(char file, char rank){
    uint8_t int_pos = (rank - '1') * 8 + (file - 'a');
    return int_pos;
}

uint8_t str_to_index(string str){
    char file = str[0];
    char rank = str[1];

    return file_rank_to_index(file, rank);
}

string index_to_str(uint8_t idx){
    char file = idx % 8 + 'a';
    char rank = idx / 8 + '1';


    string s = string(1, file) + string(1, rank);
    return s;
}