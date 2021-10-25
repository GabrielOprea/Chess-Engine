#include <string>
#include "../headers/bishop.h"
#include "../headers/table.h"

Bishop::Bishop(bool color, string position) {
    this->color = color;
    this->type = 'B';
    this->position = position;
    this->moved = false;
}


int Bishop::can_move(Table* chess_table, int offset) {
    availablePos.clear();
    int col = position[0] - 'a';
    int lin = position[1] - '1';

    vector<pair<int,int>> directions = {{-1, -1}, {-1, 1}, {1, 1}, {1, -1}};

    int flag = -1;
    for(int k = 0; k < (int) directions.size(); k++) {
        int i = lin + directions[k].first;
        int j = col + directions[k].second;
        while(is_in_range(i,j)) {
            if(chess_table->get_piece(i,j) == NULL) {
                if(chess_table->check_valid(this->get_color(), lin, col,i,j)) {
                    flag = 0;
                        chess_table->movelist.push_back(new Move(lin, col, make_pair(i - lin, j - col)));
                    //availablePos.push_back(make_pair(i - lin, j - col));
                }
                i += directions[k].first;
                j += directions[k].second;
            }
            else if (chess_table->get_piece(i,j)->get_color()
            != this->get_color()) {
                if(chess_table->check_valid(this->get_color(), lin, col,i,j)) {
                    flag = 0;
                    chess_table->movelist.push_back(new Move(lin, col, make_pair(i - lin, j - col)));
                    //availablePos.push_back(make_pair(i - lin, j - col));
                }
                break;
            }
            else {
                break;
            }
        }
    }
    return flag;
}

void Bishop::move(Table* chess_table, int offset) {
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