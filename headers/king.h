#include "piece.h"
#include <string>
#include <vector>

using namespace std;

#ifndef KING_H
#define KING_H

class King : public Piece {

    public:
        int can_move(Table* chess_table, int offset);
        void move(Table* chess_table, int offset);
        King();
        King(bool color); 
        King(bool color, string position);
};

#endif