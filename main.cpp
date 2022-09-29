#include "chess.h"


int main() {
    Board *bb;
    bb = new Board;
    bb->playGame();
    delete bb;
    return 0;
}