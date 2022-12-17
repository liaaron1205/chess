#include <iostream>
#include <string>

#include "state.h"
#include "utils.h"

using namespace std;

int main(){
    State s("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    
    s.print_board();
}