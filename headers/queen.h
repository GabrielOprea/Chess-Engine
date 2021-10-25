#include "piece.h"
#include <string>
#include <vector>

using namespace std;

#ifndef QUEEN_H
#define QUEEN_H

class Queen : public Piece {

    public:
        int can_move(Table* chess_table, int offset);
        void move(Table* chess_table, int offset);
        Queen();
        Queen(bool color); 
        Queen(bool color, string position);
};

#endif