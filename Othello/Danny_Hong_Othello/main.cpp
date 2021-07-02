#include <iostream>
#include "game.h"

using namespace std;


int main(int argc, char *argv[]){
    Game game;
    char mode;

    cout << "LET'S PLAY OTHELLO! PLEASE CHOOSE A NUMBER OPTION:" << endl
         << "A: PLAY AS BLACK (FIRST MOVE)" << endl
         << "B: PLAY AS WHITE (SECOND MOVE)" << endl
         << "C: AI VS. AI" << endl;


    for( ; ; ){
        cin >> mode;
        if((mode == 'A') || (mode == 'B') || (mode == 'C') || (mode == 'a') || (mode == 'b') || (mode == 'c')){
            game.set(mode);
            break;
        }
        else{
            cout << "INVALID OPTION! PLEASE CHOOSE A VALID OPTION: " << endl;
        }
    }

    game.play();
    return 0;
}