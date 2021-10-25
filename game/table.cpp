#include "../headers/table.h"
#define ASCII_A 97

Table::Table() {

    //Initialise a table(vector of vectors) without any chess pieces
    for (int i = 0 ; i < TABLE_SIZE ; i++) {
        vector<Piece*> crt_row;
        crt_row.resize(TABLE_SIZE);
        chess_table.push_back(crt_row);
    }

    white_checks = 0;
    black_checks = 0;

    //Set the first two rows for the white player
    set_first_row(false);
    set_pawns(false);

    //Set the first two rows for the black player
    set_first_row(true);
    set_pawns(true);

    for(int lin = 0; lin < TABLE_SIZE; lin++) { 
        for(int col = 0; col < TABLE_SIZE; col++) {
            char col_chr = col + 'a';
            string pos = to_string(lin + 1).insert(0, &col_chr, 1);
            hmap[pos] = make_pair(lin, col);
        }
    }
}


Table* Table::instance = NULL;

Table* Table::get_instance() {
    if(instance == NULL) {
        instance = new Table();
    }

    return instance;
}

Piece* Table::get_piece(string pos) {
    /*converts a string representation of the position to its corresponding
    indices, then returns the piece */
    if(pos.size() == 0) {
        cout << "AAAA" << endl;
    }
    return get_piece(to_index(pos.at(1)), to_index(pos.at(0)));
}

Piece* Table::get_piece(int lin, int col) {
    //Checks if the line and column are valid, then returns the piece
    if(col < 0 || col >= TABLE_SIZE || lin < 0 || lin >= TABLE_SIZE) {
        return nullptr;
    }
    return chess_table[lin][col];
}

void Table::set_piece(Piece* piece, string pos) {
    if(pos.size() == 0) {
        cout << "BBBB" << endl;
    }
    this->set_piece(piece, to_index(pos.at(1)), to_index(pos.at(0)));
}

void Table::set_piece(Piece* piece, int lin, int col) {
    if(col < 0 || col >= TABLE_SIZE || lin < 0 || lin >= TABLE_SIZE) {
        return;
    }
    chess_table[lin][col] = piece;
}

//Returns a pointer to the king of the desired color in the table
Piece* Table::find_king(bool bot_color, int& lin, int& col) {
    Piece* king;

    for(int i = 0; i < TABLE_SIZE; i++) {
        for(int j= 0; j < TABLE_SIZE; j++) {
            if(chess_table[i][j] != nullptr) {
                if(chess_table[i][j]->get_color() == bot_color
                && chess_table[i][j]->get_type() == 'K') {
                    king = chess_table[i][j];
                    lin = i;
                    col = j;
                    return king;
                }
            }
        }
    }
    return NULL;
}

//Performs a small castling
void Table::do_small_castling(bool bot_color) {
    Piece* king;
    int lin;
    int col;

    king = find_king(bot_color, lin, col);
    string prev_pos = king->get_position();

    char tower_col = 'f';
    char king_col = 'g';

    Piece* small_tower = this->get_piece(lin, 7);
    this->swap_tiles(small_tower->get_position(), to_string(lin + 1).insert(0, &tower_col, 1));

    string king_next_pos = to_string(lin + 1).insert(0, &king_col, 1);
    this->swap_tiles(king->get_position(), king_next_pos);

    cout << "move " << prev_pos << king_next_pos << "\n";
}

//Performs a big castling
void Table::do_big_castling(bool bot_color) {
    Piece* king;
    int lin;
    int col;

    king = find_king(bot_color, lin, col);
    string prev_pos = king->get_position();

    char tower_col = 'd';
    char king_col = 'c';

    Piece* big_tower = this->get_piece(lin, 0);
    this->swap_tiles(big_tower->get_position(), to_string(lin + 1).insert(0, &tower_col, 1));

    string king_next_pos = to_string(lin + 1).insert(0, &king_col, 1);
    this->swap_tiles(king->get_position(), king_next_pos);

    cout << "move " << prev_pos << king_next_pos << "\n";
}

//Checks if a big castling is possible
bool Table::can_big_castling(bool bot_color) {
    Piece* king;
    int lin;
    int col;

    //Must not be in check
    if(is_in_chess(bot_color))
        return false;        

    king = find_king(bot_color, lin, col);

    //King must not be moved
    if(king->get_moved() == true)
        return false;

    //No piece between king and rook
    if(chess_table[lin][col - 1] != NULL)
        return false;
    if(chess_table[lin][col - 2] != NULL)
        return false;
    if(chess_table[lin][col - 3] != NULL)
        return false;
    
    //Rook is available and not moved
    Piece* tower = this->get_piece(lin, 0);
    if(tower == NULL) return false;
    if(tower->get_moved() == true) return false;

    //All the intermediary positions are valid
    if(!check_valid(bot_color, lin, col, lin, col - 1))
        return false;
    if(!check_valid(bot_color, lin, col, lin, col - 2))
        return false;
    
    return true;
}

//Checks if a small castling is possible
bool Table::can_small_castling(bool bot_color) {
    Piece* king;
    int lin;
    int col;

    if(is_in_chess(bot_color))
        return false;        


    king = find_king(bot_color, lin, col);

    if(king->get_moved() == true)
        return false;


    if(chess_table[lin][col + 1] != NULL)
        return false;
    if(chess_table[lin][col + 2] != NULL)
        return false;
    

    Piece* tower = this->get_piece(lin, 7);
    if(tower == NULL) return false;
    if(tower->get_moved() == true) return false;


    if(!check_valid(bot_color, lin, col, lin, col + 1))
        return false;
    if(!check_valid(bot_color, lin, col, lin, col + 2))
        return false;
    

    return true;
}

bool is_in_range(int i, int j) {
    return (i >= 0) && (j >= 0) && (i < TABLE_SIZE) && (j < TABLE_SIZE);
}

//Receives a vector of relative offsets and decides if it is a rook
bool is_rook(pair<int, int> dir) {
    return dir.first == 0 || dir.second == 0;
}

//Or a bishop
bool is_bishop(pair<int, int> dir) {
    return dir.first != 0 && dir.second != 0;
}

//Checks if the king is in check by queen/bishop or rook
bool Table::check_pieces(int lin, int col, Piece* king) {
    vector<pair<int,int>> directions = {{-1, -1}, {-1, 1}, {1, 1}, {1, -1},
                                        {1,0}, {0,1}, {-1, 0}, {0, -1}};


    bool flag = false;
    for(int k = 0; k < (int) directions.size(); k++) {
        int i = lin + directions[k].first;
        int j = col + directions[k].second;
        while(is_in_range(i,j)) {
            if(this->get_piece(i,j) == nullptr) {
                i += directions[k].first;
                j += directions[k].second;
            }
            else if (this->get_piece(i,j)->get_color() != king->get_color()) {
                if((is_bishop(directions[k]) 
                    && this->get_piece(i, j)->get_type() == 'B')
                ||(is_rook(directions[k])
                    && this->get_piece(i, j)->get_type() == 'R')
                ||(this->get_piece(i, j)->get_type() == 'Q')) {

                    flag = true;
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

//Checks if the king is in check
bool Table::is_in_chess(bool bot_color) {
    Piece* king;

    int lin;
    int col;
    king = find_king(bot_color, lin, col);

    bool flag = check_pieces(lin, col, king);

    vector<pair<int, int>> relative_knight_pos = {
            {2, -1}, {2, 1}, {1, -2}, {1, 2},
            {-2, -1}, {-2, 1}, {-1, -2}, {-1, 2}
    };

    vector<pair<int, int>> relative_king_pos = {
            {-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
            {0, 1}, {1, -1}, {1, 0}, {1, 1}
    };

    //Checks the enemy king and knight
    flag |= check_piece(king, lin, col, relative_knight_pos, 'N');
    flag |= check_piece(king, lin, col, relative_king_pos, 'K');    

    int offset = bot_color ? -1 : 1;
    int pawn_lin = lin + offset;
    int left_col = col - 1;
    int right_col = col + 1;
    
    //Checks the enemy's pawns
    if(is_in_range(pawn_lin, left_col)) {
        if(chess_table[pawn_lin][left_col] != nullptr)
            if(chess_table[pawn_lin][left_col]->get_type() == 'P'
            && chess_table[pawn_lin][left_col]->get_color()
            != king->get_color()) {
                
                flag = true;
        }
    }
    if(is_in_range(pawn_lin, right_col)) {
        if(chess_table[pawn_lin][right_col] != nullptr)
            if (chess_table[pawn_lin][right_col]->get_type() == 'P'
            && chess_table[pawn_lin][right_col]->get_color()
            != king->get_color()) {
                
                flag = true;
        }
    }

    return flag;

}


bool Table::check_piece(Piece* king, int lin, int col,
vector<pair<int, int>> relative_pos, char type) {
    for(int k = 0; k < (int) relative_pos.size(); k++) {
        int i = lin + relative_pos[k].first;
        int j = col + relative_pos[k].second;
        if(is_in_range(i,j)) {
            if(this->get_piece(i,j) == NULL) {
                continue;
            }
            if(this->get_piece(i,j)->get_color() != king->get_color()
            && this->get_piece(i,j)->get_type() == type) {
                return true;
            }
        }
    }
    return false;
}

//Checks if a move from positions (lin, col) to (i, j) is valid
bool Table::check_valid(bool color, int lin, int col, int i, int j) {
    bool flag = false;
    Piece* piece1 = this->get_piece(lin, col);
    Piece* piece2 = this->get_piece(i, j);

    this->set_piece(piece1, i, j);
    this->set_piece(NULL, lin, col);
    if(!this->is_in_chess(color)) {
        flag = true;
    }
    this->set_piece(piece1, lin, col);
    this->set_piece(piece2, i, j);
    return flag;
}

void Table::print_table() {
    //Iterate through the table
    for(int i = TABLE_SIZE - 1; i >= 0; i--) {
        for(int j = 0; j < TABLE_SIZE; j++) {
            Piece* crt_piece = chess_table[i][j];
            //Check if there exists a piece on this position
            if(chess_table[i][j] == NULL) {
                cout << "---------\t";
            } else {
                char type = crt_piece->get_type();
                cout << type << crt_piece->get_color() << "(" << 
                        crt_piece->get_position() << ")("
                        << crt_piece->get_moved() << ")" << '\t';
            }
        }
        cout << endl;
    }
}

void Table::swap_tiles(int linX, int colX, int linY, int colY) {

    Piece* piece1 = get_piece(linX, colX);
    Piece* piece2 = get_piece(linY, colY);

    //If there are no pieces, return
    if(piece1 == NULL && piece2 == NULL) {
        return;
    }
    bool color1, color2;
    
    //If there exists at least one piece, then move it to the other's location
    if(piece2 != NULL){
        color2 = piece2->get_color();
    }
    if(piece1 != NULL){
        //piece1->set_position(pos2);
        color1 = piece1->get_color();
    }

    piece1->set_moved(true);


    /*If both pieces exist and are of different colors, then make sure that
    the first one will remove the second */
    if ((piece1 != NULL) && (piece2 != NULL)){
        if (color1 != color2){
            piece2 = NULL;
        }
    }

    //Put the pieces back in the table
    chess_table[linX][colX] = piece2;
    chess_table[linY][colY] = piece1;
}

void Table::swap_tiles(string pos1, int linY, int colY) {
    //Obtain the pieces we want to swap
    if(pos1.size() == 0) cout << "Designer" << endl;
    Piece* piece1 = get_piece(pos1);
    Piece* piece2 = get_piece(linY, colY);

    //If there are no pieces, return
    if(piece1 == NULL && piece2 == NULL) {
        return;
    }
    bool color1, color2;
    
    //If there exists at least one piece, then move it to the other's location
    if(piece2 != NULL){
        //piece2->set_position(pos1);
        color2 = piece2->get_color();
    }
    if(piece1 != NULL){
        //piece1->set_position(pos2);
        color1 = piece1->get_color();
    }

    piece1->set_moved(true);

    /*If both pieces exist and are of different colors, then make sure that
    the first one will remove the second */
    if ((piece1 != NULL) && (piece2 != NULL)){
        if (color1 != color2){
            piece2 = NULL;
        }
    }

    //Put the pieces back in the table
    if(pos1.size() == 0) {
        cout << "CCCC" << endl;
    }
    chess_table[to_index(pos1.at(1))][to_index(pos1.at(0))] = NULL;
    chess_table[linY][colY] = piece1;
}

int Table::swap_tiles(string pos1, string pos2) {
    //Obtain the pieces we want to swap
    if(pos1.size() == 0) cout << "Gucc" << endl;
    if(pos2.size() == 0) {
        cout << pos1 << endl;
        cout << "Balenc" << endl;
        return 0;
    }

    Piece* piece1 = get_piece(pos1);
    Piece* piece2 = get_piece(pos2);

    bool color1, color2;
    
    //If there exists at least one piece, then move it to the other's location
    if(piece2 != NULL){
        //piece2->set_position(pos1);
        color2 = piece2->get_color();
    }

    piece1->set_position(pos2);
    color1 = piece1->get_color();

    piece1->set_moved(true);

    /*If both pieces exist and are of different colors, then make sure that
    the first one will remove the second */
    if (piece2 != NULL){
        if (color1 != color2){
            piece2 = NULL;
        }
    }

    //Put the pieces back in the table
    if(pos1.size() == 0) {
        cout << "DDDD" << endl;
    }
    if(pos2.size() == 0) {
        cout << "EEEE" << endl;
    }
    chess_table[to_index(pos1.at(1))][to_index(pos1.at(0))] = NULL;
    chess_table[to_index(pos2.at(1))][to_index(pos2.at(0))] = piece1;

    return 1;
}

//Auxiliary converting function
int Table::to_index(char pos) {
    if(isalpha(pos)) return pos - 'a';
    else return pos - '1';
}

void Table::set_first_row(bool player) {
    vector<Piece*> first_row;
    //Piece order for the first row
    char pieces_name[] = {'R', 'N', 'B', 'Q', 'K', 'B', 'N', 'R'};
    char start = ASCII_A;
    for(char piece_name : pieces_name) {
        //For each piece calculate its position
        string line = player ? "8" : "1";
        string poz = line.insert(0, &start, 1);
        start ++;
        
        //Add the piece in the row
        switch(piece_name) {
            case 'R' :
                first_row.push_back(new Rook(player, poz));
                break;
            case 'N' :
                first_row.push_back(new Knight(player, poz));
                break;            
            case 'B' :
                first_row.push_back(new Bishop(player, poz));
                break;            
            case 'Q' :
                first_row.push_back(new Queen(player, poz));
                break;            
            case 'K' :
                Piece* k = new King(player, poz);
                if(!player) white_king = k;
                else black_king = k;
                first_row.push_back(k);
                break;            
        }
    }
    //Set that row as the first one for white or the last one for black
    if(!player) chess_table[0] = first_row;
    else chess_table[TABLE_SIZE - 1] = first_row;
}

void Table::set_pawns(bool player) {
    vector<Piece*> pawns_row;
    char start = ASCII_A;
    for(int i = 0; i < TABLE_SIZE; i++) {
        string line = player ? "7" : "2";
        string poz = line.insert(0, &start, 1);
        start++;
        pawns_row.push_back(new Pawn(player, poz));
    }
    if(!player) chess_table[1] = pawns_row;
    else chess_table[TABLE_SIZE - 2] = pawns_row;
}