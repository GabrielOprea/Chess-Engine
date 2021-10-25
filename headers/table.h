#include <string>
#include <vector>
#include "king.h"
#include "queen.h"
#include "pawn.h"
#include "knight.h"
#include "rook.h"
#include "bishop.h"
#include "move.h"

#include <stack>
#include <unordered_map>

#include <iostream>

using namespace std;

#ifndef TABLE_H
#define TABLE_H

class Table {
    private:
        //Table();
        static Table* instance;
        vector<vector<Piece*>> chess_table; //Matrix of pieces
        //Initialises the first row for either white or black palyer
        void set_first_row(bool player);
        
        //Initialises the pawns row(second row) for either white or black
        void set_pawns(bool player);

        //Converts a character a-h to its equivalent index
        int to_index(char pos);
        bool check_pieces(int lin, int col, Piece* king);
        bool check_piece(Piece* king, int lin, int col, vector<pair<int, int>> relative_pos, char type);

    public:
        static Table* get_instance();
        Table();
        stack<Piece*> eliminated;
        int white_checks;
        int black_checks;
        Piece* white_king;
        Piece* black_king;

        unordered_map<string, pair<int, int>> hmap;
        vector<Move*> movelist;

        Piece* get_piece(string pos);
        Piece* get_piece(int lin, int col);
        void set_piece(Piece* piece, int lin, int col);
        bool is_in_chess(bool bot_color);

        //Swaps the tiles on the specified positions in the table
        int swap_tiles(string pos1, string pos2);
        void swap_tiles(string pos1, int linY, int colY);
        void swap_tiles(int linX, int colX, int linY, int colY);
        void set_piece(Piece* piece, string pos);
        bool check_valid(bool color, int lin, int col, int i, int j);

        Piece* find_king(bool bot_color, int& lin, int& col);

        bool can_small_castling(bool bot_color); 
        void do_small_castling(bool bot_color);

        bool can_big_castling(bool bot_color); 
        void do_big_castling(bool bot_color);

        //Prints the table in a human readable format
        void print_table();
};

#endif