#include "chess.h"

using namespace std;

//the big 3: the default implementation is enough for our usage, therefore there is no need to create these specifically
//there are very few places in the code where we allocate memory dynamically and wherever it does happen we also make sure to free the memory when needed using delete.

Piece::Piece()
{
    name = "EMPTY";
    color = "NONE";
    movement = false;
}//default c'tor for Piece


Square::Square() : en_passant(), x(), y()
{
    Piece();
}//default c'tor for Square

void Square::setSpace(Square* space)
{
    piece.setColor(space->getPiece()->getColor());
    piece.setName(space->getPiece()->getName());
    if(!piece.getMovement())
        piece.setMovement();
}//set Square with information from a different square

void Square::setPieceAndColor(const string& p, const string& c)
{
    piece.setName(p);
    piece.setColor(c);
}//set both piece and color (mainly used in setting the board)

Piece * Square::getPiece()
{
    return &piece;
}//returns the Piece in said square


Board::Board() {
    setBoard();
    turn = 1;
    white_turn = true;
}//default c'tor for board, setting the start position of the board.

void Board::setBoard()
{
    square[7][0].setPieceAndColor("ROOK", "WHITE");
    square[7][1].setPieceAndColor("NIGHT", "WHITE");
    square[7][2].setPieceAndColor("BISHOP", "WHITE");
    square[7][3].setPieceAndColor("QUEEN", "WHITE");
    square[7][4].setPieceAndColor("KING", "WHITE");
    square[7][5].setPieceAndColor("BISHOP", "WHITE");
    square[7][6].setPieceAndColor("NIGHT", "WHITE");
    square[7][7].setPieceAndColor("ROOK", "WHITE");

    square[0][0].setPieceAndColor("ROOK", "BLACK");
    square[0][1].setPieceAndColor("NIGHT", "BLACK");
    square[0][2].setPieceAndColor("BISHOP", "BLACK");
    square[0][3].setPieceAndColor("QUEEN", "BLACK");
    square[0][4].setPieceAndColor("KING", "BLACK");
    square[0][5].setPieceAndColor("BISHOP", "BLACK");
    square[0][6].setPieceAndColor("NIGHT", "BLACK");
    square[0][7].setPieceAndColor("ROOK", "BLACK");

    for (int i = 0; i < 8; i++)
    {
        square[6][i].setPieceAndColor("PAWN", "WHITE");
        square[1][i].setPieceAndColor("PAWN", "BLACK");
    }

    for (int i = 2; i < 6; i++)
    {
        for (int j = 0; j < 8; j++)
            square[i][j].setPieceAndColor("EMPTY", "NONE");

    }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
        {
            square[i][j].setX(i);
            square[i][j].setY(j);
        }

}//function responsible for setting the pieces according to starting positions

void Board::printBoard() {
    string name, color;
    int row = 8;
    for (int i = 0; i < 8; i++)
    {
        cout << row-- << " ";
        for (int j = 0; j < 8; j++)
        {
            name = square[i][j].getPiece()->getName().at(0);
            color = square[i][j].getPiece()->getColor();
            char x = name.at(0);

            switch (x)
            {
                case 'K':
                    (color == "WHITE") ? cout << "k" : cout << "K";
                    break;
                case 'Q':
                    (color == "WHITE") ? cout << "q" : cout << "Q";
                    break;
                case 'B':
                    (color == "WHITE") ? cout << "b" : cout << "B";
                    break;
                case 'N':
                    (color == "WHITE") ? cout << "n" : cout << "N";
                    break;
                case 'R':
                    (color == "WHITE") ? cout << "r" : cout << "R";
                    break;
                case 'P':
                    (color == "WHITE") ? cout << "p" : cout << "P";
                    break;
                default: cout << ".";
                    break;
            }
        if(j != 7) cout << " ";
        }
        cout << endl;
    }
    cout << "  A B C D E F G H" << endl;
}//prints current state of board

void Board::next_turn() {        //print player turn
    if(white_turn)
        cout << turn++ << ") White's turn, please enter a move:" << endl;
    else
        cout << turn++ << ") Black's turn, please enter a move:" << endl;
    white_turn = !white_turn;
}//prints the the turn including which player needs to go next

void Board::playGame() {
    string input;
    while(true) {
        printBoard();
        next_turn();
        getline(cin, input);
        if(!cin)
            break;
        while(true){
            if(checkInput(input))
                if(validateMove(input))
                    break;
            getline(cin, input);
            if(!cin)
                break;
        }
        doMove(input);
        if(stalemate()){
            printBoard();
            cout << "The game is drawn due to stalemate!" << endl;
            break;
        }
        if(insufficientMaterial()){
            printBoard();
            cout << "The game is drawn due to insufficient material!" << endl;
            break;
        }
        if(checkmate()){
            printBoard();
            !white_turn ? cout << "White wins with checkmate!" << endl : cout << "Black wins with checkmate!" << endl;
            break;
        }
    }
}//function that is responsible for managing the game

bool Board::checkInput(string input) const{
    int spaces = 0, len = input.length();
    for(int i = 0; i<len; i++)
        if(input[i] == ' ')
            spaces++;
    if(spaces == 1 and input.length() == 5 and input.find_first_not_of("ABCDEFGH12345678 ") == string::npos) //check that user only entered allowed characters
        return true;
    else if(spaces == 2 and input.length() == 7){ //same for the 7 characters string for promotion
        if(input.substr(0, 5).find_first_not_of("ABCDEFGH12345678 ") == string::npos)
            if(input.substr(6).find_first_not_of("RNBQ") == string::npos)
                return true;
    }
    cerr << turn-1 << ") Invalid input; please enter a move:" << endl;
    return false;
}//function makes sure that the user entered a valid input according to insturctions

bool Board::validateMove(string input) {
    Piece * player = getSquare(input.at(0), input.at(1))->getPiece();
    if(input.substr(0, 2) != input.substr(3, 2)) { //make sure we're not telling the piece to move to its own position
        if ((player->getColor() == "WHITE" and !white_turn) or (player->getColor() == "BLACK" and white_turn)) { //verify that the player uses his own pieces
            if (input.length() == 7) { //if len 7 meaning promotion make sure that it is a valid promotion
                if (moveRook(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4)))) {
                    string new_piece;
                    switch (input.at(6)) {
                        case 'R':
                            new_piece = "ROOK";
                            break;
                        case 'N':
                            new_piece = "NIGHT";
                            break;
                        case 'B':
                            new_piece = "BISHOP";
                            break;
                        case 'Q':
                            new_piece = "QUEEN";
                            break;
                    }
                    getSquare(input.at(0), input.at(1))->getPiece()->setName(new_piece);//if promotion is legal we transfer pawn to whatever user chose
                    return true;
                }
                cerr << turn - 1 << ") Illegal move; please enter a move:" << endl;
                return false;
            }
            char piece_name = player->getName().at(0);
            switch (piece_name) { //for 5 characters input we check which piece is moving and going to the function responsible for that piece's movement to verify validity
                case 'K': //for every case we first make sure the piece can perform that move, after which we make sure that move doesn't cause check
                    if (moveKing(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4))))
                        if(!check(input))
                            return true;
                    break;
                case 'Q':
                    if (moveQueen(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4))))
                        if(!check(input))
                            return true;
                    break;
                case 'B':
                    if (moveBishop(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4))))
                        if(!check(input))
                            return true;
                    break;
                case 'N':
                    if (moveKnight(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4))))
                        if(!check(input))
                            return true;
                    break;
                case 'R':
                    if (moveRook(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4))))
                        if(!check(input))
                            return true;
                    break;
                case 'P':
                    if (getSquare(input.at(3), input.at(4))->getX() == 0 or getSquare(input.at(3), input.at(4))->getX() == 7) {
                        if (input.length() != 7)
                            break;
                    }
                    if (movePawn(getSquare(input.at(0), input.at(1)), getSquare(input.at(3), input.at(4))))
                        if(!check(input))
                            return true;
                    break;
            }
        }
    }
    cerr << turn-1 << ") Illegal move; please enter a move:" << endl;
    return false;
}//function makes sure that the valid input translates into a valid move

string Board::getInput(Square * start, Square * end) {
    string move;
    int x1 = start->getX();
    int y1 = start->getY();
    int x2 = end->getX();
    int y2 = end->getY();
    unsigned char b = abs(x1-56);
    unsigned char a = y1+65;
    unsigned char c = ' ';
    unsigned char e = abs(x2-56);
    unsigned char d = y2+65;
    move.push_back(a);
    move.push_back(b);
    move.push_back(c);
    move.push_back(d);
    move.push_back(e);
    return move;
}//translates movement from one square to another into a string

Square * Board::getSquare(char column, char row) {
    int x = abs(row-48-8);
    int y = column-65;
    return &square[x][y];
}//returns the correct square if we enter two chars as location

Square * Board::getSquare(int row, int column) {
    return &square[row][column];
}//returns the correct square if we use ints

void Board::doMove(string input) {
    Square * from_square = getSquare(input.at(0), input.at(1));
    Square * to_square = getSquare(input.at(3), input.at(4));
    if((from_square->getPiece()->getName() == "KING") and (abs(from_square->getY() - to_square->getY()) == 2)){
        Square * new_rook_square;
        Square * current_rook_square;
        int new_rook_col, cur_rook_col;
        (from_square->getY() - to_square->getY()) > 0 ? new_rook_col = -1 : new_rook_col = 1;
        (from_square->getY() - to_square->getY()) > 0 ? cur_rook_col = 0 : cur_rook_col = 7;
        new_rook_square = getSquare(from_square->getX(), from_square->getY()+new_rook_col);
        current_rook_square = getSquare(from_square->getX(), cur_rook_col);
        to_square->setSpace(from_square);
        new_rook_square->setSpace(current_rook_square);
        from_square->setPieceAndColor("EMPTY", "NONE");
        current_rook_square->setPieceAndColor("EMPTY", "NONE");
        return;
    }//special case for when we use casling
    to_square->setSpace(from_square);
    from_square->setPieceAndColor("EMPTY", "NONE");
}//function responsible for moving the pieces, all verification is done beforehand so move is already known to be legal

bool Board::moveKing(Square *thisKing, Square *thatSpace) {
    int x_distance = abs(thisKing->getX() - thatSpace->getX());
    int y_distance = abs(thisKing->getY() - thatSpace->getY());
    int journey, key;
    if(x_distance <= 1 and y_distance <= 1 and thisKing->getPiece()->getColor() != thatSpace->getPiece()->getColor())
        return true;
    if(x_distance == 0 and y_distance == 2 and !thisKing->getPiece()->getMovement() and thisKing->getPiece()->getColor() != thatSpace->getPiece()->getColor()){
        ((thisKing->getY() - thatSpace->getY()) > 0) ? journey = -1 : journey = 1;
        thatSpace->getY() == 7 ? key = 2 : key = 3;
        for (int i = 0; i < key; i++) {
            if (getSquare(thisKing->getX(), thisKing->getY()+journey)->getPiece()->getName() != "EMPTY")
                return false;
            if(i <= 2)
                if(check(getInput(thisKing, getSquare(thisKing->getX(), thisKing->getY()+journey)))) {
                    return false;
                }
            (journey > 0) ? journey++ : journey--;
        }
        return true;
    }
    return false;
}//verify that King movement is legal

bool Board::moveQueen(Square *thisQueen, Square *thatSpace) {
    int x_distance = abs(thisQueen->getX() - thatSpace->getX());
    int y_distance = abs(thisQueen->getY() - thatSpace->getY());
    int journey, extra_journey;
    if(thisQueen->getPiece()->getColor() != thatSpace->getPiece()->getColor()){
        if(x_distance == y_distance){
            ((thisQueen->getX() - thatSpace->getX()) > 0) ? journey = -1 : journey = 1;
            ((thisQueen->getY() - thatSpace->getY()) > 0) ? extra_journey = -1 : extra_journey = 1;
            for (int i = 0; i < x_distance-1; i++) {
                if (getSquare(thisQueen->getX() + journey, thisQueen->getY() + extra_journey)->getPiece()->getName() != "EMPTY")
                    return false;
                (journey > 0) ? journey++ : journey--;
                (extra_journey > 0) ? extra_journey++ : extra_journey--;
            }
            return true;
        }
        else if(y_distance == 0){
            ((thisQueen->getX() - thatSpace->getX()) > 0) ? journey = -1 : journey = 1;
            for (int i = 0; i < x_distance-1; i++) {
                if (getSquare(thisQueen->getX() + journey, thisQueen->getY())->getPiece()->getName() != "EMPTY")
                    return false;
                (journey > 0) ? journey++ : journey--;
            }
            return true;
        }
        else if(x_distance == 0) {
            ((thisQueen->getY() - thatSpace->getY()) > 0) ? journey = -1 : journey = 1;
            for (int i = 0; i < y_distance - 1; i++) {
                if (getSquare(thisQueen->getX(), thisQueen->getY() + journey)->getPiece()->getName() != "EMPTY")
                    return false;
                (journey > 0) ? journey++ : journey--;
            }
            return true;
        }
    }
    return false;
}//verify that Queen movement is legal

bool Board::moveBishop(Square *thisBishop, Square *thatSpace) {
    int x_distance = abs(thisBishop->getX() - thatSpace->getX());
    int y_distance = abs(thisBishop->getY() - thatSpace->getY());
    int journey, extra_journey;
    if(x_distance == y_distance and thisBishop->getPiece()->getColor() != thatSpace->getPiece()->getColor()){
        ((thisBishop->getX() - thatSpace->getX()) > 0) ? journey = -1 : journey = 1;
        ((thisBishop->getY() - thatSpace->getY()) > 0) ? extra_journey = -1 : extra_journey = 1;
        for (int i = 0; i < x_distance-1; i++) {
            if (getSquare(thisBishop->getX() + journey, thisBishop->getY() + extra_journey)->getPiece()->getName() != "EMPTY")
                return false;
            (journey > 0) ? journey++ : journey--;
            (extra_journey > 0) ? extra_journey++ : extra_journey--;
        }
        return true;
    }
    return false;
}//verify that Bishop movement is legal

bool Board::moveKnight(Square *thisKnight, Square *thatSpace) {
    int x_distance = abs(thisKnight->getX() - thatSpace->getX());
    int y_distance = abs(thisKnight->getY() - thatSpace->getY());
    return (((x_distance == 2 and y_distance == 1) or (x_distance == 1 and y_distance == 2)) and thisKnight->getPiece()->getColor() != thatSpace->getPiece()->getColor());
}//verify that Knight movement is legal

bool Board::moveRook(Square *thisRook, Square *thatSpace) {
    int x_distance = abs(thisRook->getX() - thatSpace->getX());
    int y_distance = abs(thisRook->getY() - thatSpace->getY());
    int journey;
    if(thisRook->getPiece()->getColor() != thatSpace->getPiece()->getColor()){
        if(y_distance == 0){
            ((thisRook->getX() - thatSpace->getX()) > 0) ? journey = -1 : journey = 1;
            for (int i = 0; i < x_distance-1; i++) {
                if (getSquare(thisRook->getX() + journey, thisRook->getY())->getPiece()->getName() != "EMPTY")
                    return false;
                (journey > 0) ? journey++ : journey--;
            }
            return true;
        }
        else if(x_distance == 0) {
            ((thisRook->getY() - thatSpace->getY()) > 0) ? journey = -1 : journey = 1;
            for (int i = 0; i < y_distance - 1; i++) {
                if (getSquare(thisRook->getX(), thisRook->getY() + journey)->getPiece()->getName() != "EMPTY")
                    return false;
                (journey > 0) ? journey++ : journey--;
            }
            return true;
        }
    }
    return false;
}//verify that Rook movement is legal

bool Board::movePawn(Square *thisPawn, Square *thatSpace) {
    string enemy = thatSpace->getPiece()->getName();
    int x_distance = abs(thisPawn->getX() - thatSpace->getX());
    int y_distance = abs(thisPawn->getY() - thatSpace->getY());
    bool check_a = thisPawn->getPiece()->getColor() == "WHITE" and thisPawn->getX()>thatSpace->getX();
    bool check_b = thisPawn->getPiece()->getColor() == "BLACK" and thisPawn->getX()<thatSpace->getX();
    if((check_a or check_b) and (thisPawn->getPiece()->getColor() != thatSpace->getPiece()->getColor())){
        if (x_distance == 2 and y_distance == 0 and !thisPawn->getPiece()->getMovement() and enemy == "EMPTY") {
            if (thisPawn->getPiece()->getColor() == "WHITE" and
                getSquare(thisPawn->getX() - 1, thisPawn->getY())->getPiece()->getName() == "EMPTY") {
                if (thisPawn->getX() - thatSpace->getX() == 2) {
                    getSquare(thisPawn->getX()-1, thisPawn->getY())->setEnp(turn);
                    return true;
                }
            } else if (thisPawn->getPiece()->getColor() == "BLACK" and getSquare(thisPawn->getX() + 1, thisPawn->getY())->getPiece()->getName() == "EMPTY") {
                if (thatSpace->getX() - thisPawn->getX() == 2) {
                    getSquare(thisPawn->getX()+1, thisPawn->getY())->setEnp(turn);
                    return true;
                }
            }
        }
        if (x_distance == 1) {
            if (y_distance == 0 and enemy == "EMPTY")
                return true;
            if (y_distance == 1 and enemy != "EMPTY")
                return true;
            else if (y_distance == 1 and thatSpace->getEnp() == getTurn()-1) {
                if (thisPawn->getPiece()->getColor() == "WHITE" and thatSpace->getX()==2) {
                    getSquare(thatSpace->getX()+1, thatSpace->getY())->setPieceAndColor("EMPTY", "NONE");
                    return true;
                }
                else if (thisPawn->getPiece()->getColor() == "BLACK" and thatSpace->getX()==5) {
                    getSquare(thatSpace->getX()-1, thatSpace->getY())->setPieceAndColor("EMPTY", "NONE");
                    return true;
                }
            }
        }
    }
    return false;
}//verify that Pawn movement is legal

Square * Board::kingSquare(const string& player_color){
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if(getSquare(i, j)->getPiece()->getName() == "KING" and getSquare(i, j)->getPiece()->getColor() == player_color){
                return getSquare(i, j);
            }
        }
    }
    return NULL;
}//returns whatever square the king is on, used for check and checkmate verifications

Square ** Board::remainingEnemySquares(const string& player_color){
    Square ** arr;
    arr = new Square * [16];
    for(int t = 0; t<16; t++)
        arr[t] = NULL;
    int counter = 0;
    string enemy_color;
    player_color == "WHITE" ? enemy_color = "BLACK" : enemy_color = "WHITE";
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            if(getSquare(i, j)->getPiece()->getColor() == enemy_color){
                arr[counter++] = getSquare(i, j);
            }
        }
    }
    return arr;
}//returns an array of pointers to squares of all living enemy pieces

Square ** Board::remainingKingMoves(Square * kingSquare){
    Square ** arr;
    arr = new Square * [9];
    for(int t = 0; t<9; t++)
        arr[t] = NULL;
    int s = 0;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int king_x = kingSquare->getX()+i-1, king_y = kingSquare->getY()+j-1;
            if (king_x < 0 or king_x > 7 or king_y < 0 or king_y > 7)
                continue;
            if (king_x == kingSquare->getX() and king_y == kingSquare->getY())
                continue;
            arr[s++] = getSquare(king_x, king_y);
            }
        }
    return arr;
}//returns possible moves for king defending from check

bool Board::check(const string& input) {
    Board cb = *this; //use a new board to test move on
    string player_color;
    if(!input.empty()) { //if we recieve input behaves normally, otherwise made to check whether defender is in check in case of empty string
        cb.doMove(input); //we perform said move and then check if we're in check, if so we know move wasn't legal
        !white_turn ? player_color = "WHITE" : player_color = "BLACK";
    }
    else
        !white_turn ? player_color = "BLACK" : player_color = "WHITE";
    Square * king = cb.kingSquare(player_color);
    Square ** enemy_pieces = cb.remainingEnemySquares(player_color);
    for (int i = 0; i < 16; i++) { //goes over all enemy pieces, checking whether any of them can reach the king in a legal move
            Square * attacker = enemy_pieces[i];
            if(enemy_pieces[i] == NULL)
                break;
            char initial = enemy_pieces[i]->getPiece()->getName().at(0);
            switch(initial)
            {
                case 'P':
                    if(cb.movePawn(attacker, king)){
                        delete [] enemy_pieces;
                        return true;
                    }
                    break;
                case 'K':
                    if(cb.moveKing(attacker, king)) {
                        delete [] enemy_pieces;
                        return true;
                    }
                    break;
                case 'Q':
                    if(cb.moveQueen(attacker, king)){
                        delete [] enemy_pieces;
                        return true;
                    }
                    break;
                case 'N':
                    if(cb.moveKnight(attacker, king)){
                        delete [] enemy_pieces;
                        return true;
                    }
                    break;
                case 'B':
                    if(cb.moveBishop(attacker, king)) {
                        delete [] enemy_pieces;
                        return true;
                    }
                    break;
                case 'R':
                    if(cb.moveRook(attacker, king)) {
                        delete [] enemy_pieces;
                        return true;
                    }
                    break;
            }
    }
    delete [] enemy_pieces;
    return false;
}//checks whether or not enemy \ player king is in check (depends on input)

bool Board::checkmate() {
    if(check("")){
        string attacker_color, defender_color, input;
        !white_turn ? attacker_color = "WHITE" : attacker_color = "BLACK";
        !white_turn ? defender_color = "BLACK" : defender_color = "WHITE";
        Square * defendingKing = kingSquare(defender_color);
        Square ** defenders = remainingEnemySquares(attacker_color);
        Square ** kingMoves = remainingKingMoves(defendingKing);
        Board cb;
        for (int i = 0; i < 9; i++) {
            if(kingMoves[i] == NULL)
                break;
            cb = *this;
            input = getInput(getSquare(defendingKing->getX(), defendingKing->getY()), getSquare(kingMoves[i]->getX(), kingMoves[i]->getY()));
            cb.doMove(input);
            if (!cb.check("")) {
                delete [] defenders;
                return false;
            }
        }//we first check if the king can make any legal moves that will remove him from check
        delete [] kingMoves;
        if(!defendingKing->getPiece()->getMovement()){
            cb = *this;
            int row, col = 0;
            !white_turn ? row = 0 : row = 7;
            for (int i = 0; i < 2; i++) {
                if(moveKing(defendingKing, getSquare(row, col))) {
                    input = getInput(defendingKing, getSquare(row, col));
                    if (!cb.check(input)) {
                        cb.doMove(input);
                        if (!cb.check("")){
                            delete [] defenders;
                            return false;
                        }
                    }
                }
                col = 6;
            }
        }//we also check if casling is possible to escape said check
        for (int i = 0; i < 16; i++) {//lastly in case king can't move, we check every other piece on his team and see if their movement can help him avoid check
            if(defenders[i] == NULL)
                break;
            else if(defenders[i]->getPiece()->getName() == "KING")
                continue;
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    cb = *this;
                    Square * target = &cb.square[j][k];
                    input = getInput(defenders[i], target);
                    char initial = defenders[i]->getPiece()->getName().at(0);
                    switch(initial) {
                        case 'P':
                            if (movePawn(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'K':
                            if (moveKing(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'Q':
                            if(moveQueen(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'N':
                            if(moveKnight(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'B':
                            if(moveBishop(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'R':
                            if(moveRook(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                    }
                }
            }
        }
        delete [] defenders;
        return true;
    }
    return false;
}//checks for checkmate by checking whether or not any move of player pieces can remove the check status

bool Board::stalemate(){
    string opponent_color;
    !white_turn ? opponent_color = "WHITE" : opponent_color = "BLACK";
    if(!check("")){
        Square ** defenders = remainingEnemySquares(opponent_color);
        Board cb = *this;
        for (int i = 0; i < 16; i++) {//checks entire board for all player pieces, any legal move will return false
            if(defenders[i] == NULL)
                break;
            for (int j = 0; j < 8; j++) {
                for (int k = 0; k < 8; k++) {
                    cb = *this;
                    Square * target = &cb.square[j][k];
                    string input = getInput(defenders[i], target);
                    char initial = defenders[i]->getPiece()->getName().at(0);
                    switch(initial) {
                        case 'P':
                            if (movePawn(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'K':
                            if (moveKing(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'Q':
                            if(moveQueen(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'N':
                            if(moveKnight(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'B':
                            if(moveBishop(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                        case 'R':
                            if(moveRook(defenders[i], target)) {
                                cb.doMove(getInput(defenders[i], target));
                                if (!cb.check("")) {
                                    delete [] defenders;
                                    return false;
                                }
                            }
                            break;
                    }
                }
            }
        }
        delete [] defenders;
        return true;
    }
    return false;
}//if we're not in check, attempts to see if there are ANY legal moves

bool Board::insufficientMaterial(){
    Square ** white_pieces = remainingEnemySquares("BLACK");
    Square ** black_pieces = remainingEnemySquares("WHITE");
    int black_counter = 0, white_counter = 0;
    for (int i = 0; i < 3; i++) {
        if(white_pieces[i] == NULL)
            break;
        string white_piece = white_pieces[i]->getPiece()->getName();
        if(white_piece == "QUEEN" or white_piece == "ROOK" or white_piece == "PAWN") {//any of these pieces cancels insufficient material therefore false right away
            delete [] white_pieces;
            delete [] black_pieces;
            return false;
        }
        white_counter++;
    }
    for (int i = 0; i < 3; i++) {//we perform two different for loops so as to not cause issues with pieces counting
        if(black_pieces[i] == NULL)
            break;
        string black_piece = black_pieces[i]->getPiece()->getName();
        if(black_piece == "QUEEN" or black_piece == "ROOK" or black_piece == "PAWN") {
            delete [] white_pieces;
            delete [] black_pieces;
            return false;
        }
        black_counter++;
    }
    if(white_counter > 2 or black_counter > 2) {//if any counter is above 2 it means we have more than 2 pieces, therefore not insufficient material
        delete [] white_pieces;
        delete [] black_pieces;
        return false;
    }
    delete [] white_pieces;
    delete [] black_pieces;
    return true;
}//check if both players have only remaining pieces that lead to insufficient material