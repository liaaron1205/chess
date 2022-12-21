#include "engine.h"

#include <iostream>
#include <string> 

using namespace std;

int main(int arc, char* argv[]){
    string color = argv[1];

    Engine e(color);
    while (1){
        bool b = e.run();
        if (b) break;
    }
}