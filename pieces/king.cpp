#include <string>
#include "../headers/king.h"
#include "../headers/table.h"


King::King(bool color, string position) {
    this->color = color;
    this->type = 'K';
    this->position = position;
    this->moved = false;
}

int King::can_move(Table* chess_table, int offset) {
    availablePos.clear();
    int col = position[0] - 'a';
    int lin = position[1] - '1';

    //Relative offsets for the king
    vector<pair<int, int>> relative_pos = {
            {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
            {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };

    int flag = -1;
    for(int k = 0; k < (int) relative_pos.size(); k++) {
        //Obtain all 8 positions a king can be moved
        int i = lin + relative_pos[k].first;
        int j = col + relative_pos[k].second;
        if(is_in_range(i,j)) {
            //If the position is valid, add it in the vector
            if(chess_table->get_piece(i,j) == NULL
            || chess_table->get_piece(i,j)->get_color() != this->get_color()) {
                if(chess_table->check_valid(this->get_color(), lin, col,i,j)) {
                    flag = 0;
                    //if(is_in_range(lin + relative_pos[k].first, col + relative_pos[k].second))
                        chess_table->movelist.push_back(new Move(lin, col, relative_pos[k]));
                    //availablePos.push_back(relative_pos[k]);
                }
            }
        }
    }
    return flag;

}

void King::move(Table* chess_table, int offset) {
    srand(time(NULL));
    int size = availablePos.size();
    int randPos = rand() % size;
    string prev_position = this->get_position();
    int col = position[0] - 'a' + availablePos[randPos].second;
    int lin = position[1] - '1' + availablePos[randPos].first;
    char col_chr = col + 'a';
    string pos_final = to_string(lin + 1).insert(0, &col_chr, 1);
    chess_table->swap_tiles(this->get_position(), pos_final);
    cout << "move " << prev_position << pos_final << "\n";
}