#include <iostream>
#include <string>

#include "state.h"
#include "utils.h"

using namespace std;

int main(){
    State s("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    
    s.print();
}