#include "piece.h"
#include <string>

using namespace std;

#ifndef PAWN_H
#define PAWN_H

class Pawn : public Piece {

    public:
        bool en_passant;
        void promote(Table* chess_table, int offset);
        int can_move(Table* chess_table, int offset);
        void move(Table* chess_table, int offset);
        Pawn();
        Pawn(bool color); 
        Pawn(bool color, string position);
        bool check_diagonal(Piece* piece_diag,
                              Table* chess_table, int offset, int lin, int col, int dir);
        bool check_diagonal_and_move(Piece* piece_diag,
                        Table* chess_table, int offset, int lin, int col, int dir);
        bool check_next_and_move(Piece* piece, Table* chess_table, int offset, int dir);
        bool check_first_move(int lin);
        bool get_en_passant();
        void set_en_passant(bool en_passant);
};

#endif