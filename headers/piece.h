#include <string>
#include <vector>

using namespace std;

#ifndef PIECE_H
#define PIECE_H

#define TABLE_SIZE 8


class Table;

class Piece {
    public:
        bool color; //0 - WHITE, 1 - BLACK
        char type; //K-King, Q-Queen, R-Rook, N-Knight, B-Bishop, 0-Pawn
        string position; //The position on the table respresented as a string
        
        //Vector of available positions represented as offsets
        vector<pair<int, int>> availablePos;
        //Whether the piece has been moved before or not
        bool moved;

    public:
        //Abstract methods
        virtual int can_move(Table* chess_table, int offset) = 0;
        virtual void move(Table* chess_table, int offset) = 0;
        
        //Constructors
        Piece();
        Piece(bool color, char type, string position);
        
        //Getters and setters
        bool get_color();
        char get_type();
        void set_type(char type);
        void set_position(string position);
        string get_position();
        bool is_in_range(int i, int j);
        void set_moved(bool flag);
        bool get_moved();
};

#endif