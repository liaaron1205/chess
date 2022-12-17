#pragma once

#include <string>

using namespace std;

enum Color{
    black,
    white
};

enum CastleSide{
    
}

enum Piece{
    pawn,
    knight,
    bishop,
    rook,
    queen,
    king
};

const int NUM_PIECES = 6;

uint8_t file_rank_to_index( char file, char rank );

uint8_t str_to_index( string pos );

string index_to_str( uint8_t idx );