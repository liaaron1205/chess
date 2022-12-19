#include <iostream>
#include <string>
#include <vector>

#include "state.h"
#include "utils.h"

using namespace std;

int main(){
    
    //State s("rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2");
    //State s("rnbqkbnr/pppppppp/8/8/8/4p3/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    State s("8/5P2/8/8/8/8/8/8 w - - 0 1");
    //State s;

    s.print();

    vector<string> a, b;

    State s1(s, "f7f8n");
    s1.print();
    // s.generate_moves(a, b);

    // cout << a.size() << endl;
    for (auto x: a) cout << x << endl;
}