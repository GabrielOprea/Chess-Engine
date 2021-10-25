#include "piece.h"
#include <string>
#include <vector>

using namespace std;

#ifndef BISHOP_H
#define BISHOP_H

class Bishop : public Piece {

    public:
        int can_move(Table* chess_table, int offset);
        void move(Table* chess_table, int offset);
        Bishop();
        Bishop(bool color); 
        Bishop(bool color, string position);
};

#endif