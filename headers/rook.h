#include "piece.h"
#include <string>
#include <vector>

using namespace std;

#ifndef ROOK_H
#define ROOK_H

class Rook : public Piece {

    public:
        int can_move(Table* chess_table, int offset);
        void move(Table* chess_table, int offset);
        Rook();
        Rook(bool color); 
        Rook(bool color, string position);
};

#endif