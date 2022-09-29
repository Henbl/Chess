#ifndef EXERCISE_2_CHESS_H
#define EXERCISE_2_CHESS_H

#include <iostream>
#include <cmath>

using namespace std;

class Piece
{
    bool movement;
    string name;
    string color;
public:
    Piece();
    string getName() const { return name; }
    string getColor() const { return color; }
    bool getMovement() const { return movement; }
    void setName(const string& new_name) { name = new_name; }
    void setColor(const string& new_color) { color = new_color; }
    void setMovement() { movement = true; }
};


class Square
{
    int en_passant;
    Piece piece;
    int x, y;
public:
    Square();
    void setSpace(Square*);
    void setPieceAndColor(const string& p, const string& c);
    Piece * getPiece();
    void setX(int next) { x = next; }
    void setY(int next) { y = next; }
    void setEnp(int next) { en_passant = next; }
    int getX() const { return x; }
    int getY() const { return y; }
    int getEnp() const { return en_passant; }
};

class Board
{
    Square square[8][8];
    int turn;
    bool white_turn;
    void setBoard();
    void next_turn();
    bool checkInput(string input) const;
    bool validateMove(string input);
    static string getInput(Square * start, Square * end);
    bool moveKing(Square* thisKing, Square* thatSpace);
    bool moveQueen(Square* thisQueen, Square* thatSpace);
    bool moveBishop(Square* thisBishop, Square* thatSpace);
    static bool moveKnight(Square* thisKnight, Square* thatSpace);
    bool moveRook(Square* thisRook, Square* thatSpace);
    bool movePawn(Square* thisPawn, Square* thatSpace);
    bool check(const string& input);
    bool checkmate();
    bool insufficientMaterial();
    bool stalemate();
    Square * kingSquare(const string& player_color);
    Square ** remainingEnemySquares(const string& player_color);
    Square ** remainingKingMoves(Square * kingSquare);
public:
    Board();
    void printBoard();
    void playGame();
    Square * getSquare(char row, char column);
    Square * getSquare(int row, int column);
    void doMove(string input);
    int getTurn() const { return turn; }

};

#endif //EXERCISE_2_CHESS_H