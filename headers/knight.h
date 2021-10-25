#include "piece.h"
#include <string>
#include <vector>

using namespace std;

#ifndef KNIGHT_H
#define KNIGHT_H

class Knight : public Piece {

    public:
        int can_move(Table* chess_table, int offset);
        void move(Table* chess_table, int offset);
        Knight();
        Knight(bool color); 
        Knight(bool color, string position);
};

#endif