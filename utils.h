#pragma once

#include <string>

using namespace std;

enum class Player{
    Black,
    White
};

enum class CastleSide{
    Queen,
    King
};

enum class Piece{
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King
};

const int NUM_PIECES = 6;

uint8_t file_rank_to_idx( char file, char rank );
string file_rank_to_move( char from_file, char from_rank, char to_file, char to_rank );

uint8_t str_to_idx( string pos );

uint64_t    bitset( uint8_t idx );
uint64_t    bituns( uint8_t idx );
bool        bitchk( uint64_t val, uint8_t idx );

string idx_to_str( uint8_t idx );
string idx_to_move( uint8_t from, uint8_t to );

char get_file( string pos );
char get_rank( string pos );

char get_file( uint8_t idx );
char get_rank( uint8_t idx );

Player other_player( Player p );