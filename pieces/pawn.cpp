#include <string>
#include "../headers/pawn.h"
#include "../headers/table.h"

bool Pawn::check_diagonal(Piece* piece_diag, Table* chess_table, int offset,
int lin, int col, int dir) {
   
    //If yes, move the piece, update its position, remove the enemy piece
    //and return true
    bool flag = false;
    if(piece_diag != nullptr && piece_diag->get_color() != this->get_color()) {

        if(chess_table->check_valid(this->get_color(), lin, col, lin + offset,
        col + dir)) {
            flag = true;
        }
    }

    if(flag == true)
        chess_table->movelist.push_back(new Move(lin, col, make_pair(offset, dir)));
        //availablePos.push_back(make_pair(offset, dir));
    return flag;
}

bool Pawn::check_diagonal_and_move(Piece* piece_diag, Table* chess_table,
int offset, int lin, int col, int dir) {

    //If yes, move the piece, update its position, remove the enemy piece
    //and return true
    bool flag = false;
    if(piece_diag != nullptr && piece_diag->get_color() != this->get_color()) {

        if(chess_table->check_valid(this->get_color(), lin, col, lin + offset,
        col + dir)) {
            flag = true;
        }


        if(flag) {
            chess_table->set_piece(NULL, lin + offset, col + dir);
            string prev_position = this->get_position();
            chess_table->swap_tiles(this->get_position(),
            piece_diag->get_position());

            cout << "move " << prev_position << piece_diag->get_position()
            << "\n";
        }
    }
    return flag;
}

//Check if there is a pawn sideways
bool Pawn::check_next_and_move(Piece* piece, Table* chess_table, int offset,
int dir) {

    int col = (this->get_position())[0] - 'a';
    int lin = (this->get_position())[1] - '1';
    bool flag = false;
    if(piece != nullptr && piece->get_color() != this->get_color()) {
        if(chess_table->check_valid(this->get_color(), lin, col, lin,
        col + dir)) {
            flag = true;
        }
        if(flag) {
            if(piece->get_type() == 'P' && ((Pawn*) piece)->get_en_passant()) {
                chess_table->set_piece(NULL, lin, col + dir);        
                char coloana = (col + dir) + 'a';
                string prev_position = this->get_position();
                string final_position = to_string(lin + offset + 1).insert(0,
                &coloana, 1);
                chess_table->swap_tiles(prev_position, final_position);
                cout << "move " << prev_position << final_position << "\n";
            }
            else {
                flag = false;
            }
        }
    }
    return flag;
}

//Checks if there is a pawn on line 1 or 6
bool Pawn::check_first_move(int lin) {
    if(this->get_color() && (lin == 6)) {
        return true;
    }
    if(!this->get_color() && (lin == 1)) {
        return true;
    }
    return false;
}

Pawn::Pawn(bool color, string position) {
    this->color = color;
    this->type = 'P';
    this->position = position;
    this->moved = false;
    this->en_passant = false;
}

bool Pawn::get_en_passant() {
    return en_passant;
}

void Pawn::set_en_passant(bool en_passant) {
    this->en_passant = en_passant;
}

int Pawn::can_move(Table* chess_table, int offset) {
    //availablePos.clear();
    int col = position[0] - 'a';
    int lin = position[1] - '1';
    //Get the pieces on both diagonals of the piece
    Piece* piece1_diag = col != 7 ? chess_table->get_piece(lin + offset,
    col + 1) : nullptr;
    
    Piece* piece2_diag = col != 0 ? chess_table->get_piece(lin + offset,
    col - 1) : nullptr;

    int result = -1;
    if(check_diagonal(piece1_diag, chess_table, offset, lin, col, 1)) result = 0;
    if(check_diagonal(piece2_diag, chess_table, offset, lin, col, -1)) result = 0;



    // if(!check_diagonal(piece1_diag, chess_table, offset,
    //     lin, col, 1)) {
    //     if(!check_diagonal(piece2_diag, chess_table,
    //     offset, lin, col, -1)) {
    if(chess_table->get_piece(lin + offset, col) == nullptr) {
        int flag = -1;
        if(chess_table->check_valid(this->get_color(), lin, col,
        lin + offset, col)) {
            flag = 0;
            result = 0;
            chess_table->movelist.push_back(new Move(lin, col, make_pair(offset, 0)));
            //availablePos.push_back(make_pair(offset, 0));
        }
        if(flag == -1) return result;

        //Checks if the pawn can move one position and not enter check
        if(check_first_move(lin)) {
            //For lookup in the internal table
            int first = this->get_color() ? -1 : 1;
            if(chess_table->get_piece(lin + offset + first, col) == nullptr &&chess_table->check_valid(this->get_color(), lin, col,
            lin + offset + first, col)) {
                result = 0;
               chess_table->movelist.push_back(new Move(lin, col, make_pair(offset + first, 0)));
                //availablePos.push_back(make_pair(offset + first, 0));
            }
        }
        return result;
    }
    else {
        col = (this->get_position())[0] - 'a';
        lin = (this->get_position())[1] - '1';

        if(chess_table->get_piece(lin + offset, col) != nullptr) {
            return -1;
        }
    }
    return 0;
}

void Pawn::move(Table* chess_table, int offset) {
    int col = (this->get_position())[0] - 'a';
    int lin = (this->get_position())[1] - '1';

    Piece* piece1_diag = col != 7 ? chess_table->get_piece(lin + offset,
                                                           col + 1) : nullptr;

    Piece* piece2_diag = col != 0 ? chess_table->get_piece(lin + offset,
                                                           col - 1) : nullptr;

    Piece* piece1_next = is_in_range(lin, col + 1) ?
    chess_table->get_piece(lin, col + 1) : nullptr;
    
    Piece* piece2_next = is_in_range(lin, col - 1) ?
    chess_table->get_piece(lin, col - 1) : nullptr;

    if(!check_diagonal_and_move(piece1_diag, chess_table, offset,
                       lin, col, 1)) {
        if (!check_diagonal_and_move(piece2_diag, chess_table,
                            offset, lin, col, -1)) {
            if(!check_next_and_move(piece1_next, chess_table, offset, 1)) {
                if(!check_next_and_move(piece2_next, chess_table, offset,-1)) {

                    char coloana = col + 'a';
                    string poz_final = to_string(lin + offset + 1)
                    .insert(0, &coloana, 1);
                    
                    if(check_first_move(lin)) {
                        int first = this->get_color() ? -1 : 1;
                        if(chess_table->get_piece(lin + offset + first, col)
                        == nullptr
                            && chess_table->check_valid(this->get_color(),
                            lin, col, lin + offset + first, col)) {
                            
                            bool random = rand() % 2;
                            if(random) {
                                first = this->get_color() ? 0 : 2;
                                poz_final = to_string(lin + offset + first)
                                .insert(0, &coloana, 1);
                            }
                        }
                    }
                    string prev_position = this->get_position();
                    chess_table->swap_tiles(this->get_position(), poz_final);
                    cout << "move " << prev_position << poz_final;
                }
            }
        }
    }

    string crt_pos = this->get_position();
    int crt_lin = crt_pos[1] - '0';

    //Convert the pawn to another class if it promoted
    if(crt_lin == 1 || crt_lin == 8) {
        vector<char> possible_promoting = {'q', 'r', 'n', 'b'};
        int index = rand() % possible_promoting.size();
        Piece* new_piece;
        bool bot_color = (offset == 1) ? false : true;
        cout << possible_promoting[index];
        switch(possible_promoting[index]) {
            case 'q':
                new_piece = new Queen(bot_color, crt_pos);
                break;
            case 'n':
                new_piece = new Knight(bot_color, crt_pos);
                break;
            case 'b':
                new_piece = new Bishop(bot_color, crt_pos);
                break;
            case 'r':
                new_piece = new Rook(bot_color, crt_pos);
                break;
        }
        chess_table->set_piece(new_piece, crt_pos);
    }
    cout << "\n";
}