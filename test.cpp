#include <iostream>
#include <string>
#include <vector>

#include "state.h"
#include "utils.h"

using namespace std;

uint64_t count = 0;



uint64_t f( State s, int de){
    uint64_t count = 0;
    if (de == 0){
        count++;
        return 1;
    }
    vector<string> a, b;
    vector<State> c, d;
    s.generate_moves(a, b, c, d);
    for ( auto x : c ) count += f(x, de-1);
    return count;
}

void f_initial ( State s, int de ){
    vector<string> a, b;
    vector<State> c, d;
    s.generate_moves(a, b, c, d);
    for ( uint8_t i = 0; i< a.size(); i++ ){
        cout << a[i] << " " << to_string(f(c[i], de-1)) << " " << c[i].to_fen() << endl;
    }
}

int main(){ 
    int x;
    cin >> x;
    // string inp;
    // getline(cin, inp);
    
    // State s(inp);
    State s("r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 10");
    cout << to_string(f(s, x)) << endl;
    // f_initial( s, x );
    // cout << to_string(count) << endl;
    /*
    string inp;
    getline(cin, inp);
    
    State s(inp);

    // cout << s.to_fen() << endl;

    vector<string> a, b;
    vector<State> c, d;

    s.generate_moves(a, b, c, d);

    for ( auto x: c ) cout << x.to_fen() << endl;
    */
}