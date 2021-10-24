#include <iostream>
#include <cstring>
#include <fstream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
#include <stdlib.h>
#include <time.h>
#include <unordered_map>

#include "headers/piece.h"
#include "headers/table.h"
#include "headers/move.h"
#include "limits.h"

#define DEPTH 6
#define EXACT 0
#define LOWERBOUND 1
#define UPPERBOUND 2
#define pass ((void)0)

Table* staticTable;
bool staticPlayer;

//Values for calculating the positional heuristic
int pst_pawn[64] = {
  10, 10, 10,  10, 10, 10, 10, 10,
  5, 5, 5, 5,   5,  5,  5, 5,
  3, 3, 3, 3,   3,  3,  3, 3,
  0, 0, 0, 0,  15,  0,  0, 0,
  0, 0, 0, 0,  10,  0,  0, 0,
  0, 0, 0, 0,   5,  0,  0, 0,
  0, 0, 0, 25,-25, 25,  0, 0,
  0, 0, 0, 0,   0,  0,  0, 0
};

int pst_knight[64] = {
  -40, -25, -25, -25, -25, -25, -25, -40,
  -30, 0, 0, 0, 0, 0, 0, -30,
  -30, 0, 0, 0, 0, 0, 0, -30,
  -30, 0, 0, 25, 25, 0, 0, -30,
  -30, 0, 0, 25, 25, 0, 0, -30,
  -30, 0, 10, 0, 25, 10, 0, -30,
  -30, 0, 25, 25, 25, 25, 0, -30,
  -40, -30, -25, -25, -25, -25, -30, -40
};

int pst_bishop[64] = {
  -10, 0, 0, 0, 0, 0, 0, -10,
  -10, 15, 0, 0, 0, 0, 15, -10,
  -10, 0, 15, 0, 0, 15, 0, -10,
  -10, 0, 0, 20, 20, 0, 0, -10,
  -10, 0, 15, 20, 20, 15, 0, -10,
  -10, 0, 15, 0, 0, 15, 0, -10,
  -10, 15, 0, 0, 20, 0, 15, -10,
  -10, -20, -20, -20, -20, -20, -20, -10
};

int pst_rook[64] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  10, 10, 10, 10, 10, 10, 10, 10,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  5, 5, 5, 5, 5, 5, 5, 5,
  10, 10, 10, 10, 10, 10, 10, 10,
  0, 0, 0, 10, 10, 0, 0, 0
};

int pst_king[64] = {
  -25, -25, -25, -25, -25, -25, -25, -25,
  -25, -25, -25, -25, -25, -25, -25, -25,
  -25, -25, -25, -25, -25, -25, -25, -25,
  -25, -25, -25, -25, -25, -25, -25, -25,
  -25, -25, -25, -25, -25, -25, -25, -25,
  -25, -25, -25, -25, -25, -25, -25, -25,
  -50, -50, -50, -50, -50, -50, -50, -50,
  20, 25, -15, -15, -15, -15, 25, 20
};

int flip[64] = {
  56, 57, 58, 59, 60, 61, 62, 63,
  48, 49, 50, 51, 52, 53, 54, 55,
  40, 41, 42, 43, 44, 45, 46, 47,
  32, 33, 34, 35, 36, 37, 38, 39,
  24, 25, 26, 27, 28, 29, 30, 31,
  16, 17, 18, 19, 20, 21, 22, 23,
  8, 9, 10, 11, 12, 13, 14, 15,
  0, 1, 2, 3, 4, 5, 6, 7
};

typedef struct entry {
    int flag;
    int value;
    int depth;
} ttEntry;

unsigned long long int ZobristTable[8][8][12];
unordered_map<unsigned long long int, ttEntry> transpositionTable;
unsigned long long int hvalue = 0;
mt19937 mt(01234567);
unsigned long long int randomInt()
{
    uniform_int_distribution<unsigned long long int>
                                 dist(0, UINT64_MAX);
    return dist(mt);
}

int indexOf(Piece* piece)
{
    if (piece->get_type() =='P' && piece->get_color() == false)
        return 0;
    if (piece->get_type() =='N' && piece->get_color() == false)
        return 1;
    if (piece->get_type() =='B' && piece->get_color() == false)
        return 2;
    if (piece->get_type() =='R' && piece->get_color() == false)
        return 3;
    if (piece->get_type() =='Q' && piece->get_color() == false)
        return 4;
    if (piece->get_type() =='K' && piece->get_color() == false)
        return 5;
    if (piece->get_type() =='P' && piece->get_color() == true)
        return 6;
    if (piece->get_type() =='N' && piece->get_color() == true)
        return 7;
    if (piece->get_type() =='B' && piece->get_color() == true)
        return 8;
    if (piece->get_type() =='R' && piece->get_color() == true)
        return 9;
    if (piece->get_type() =='Q' && piece->get_color() == true)
        return 10;
    if (piece->get_type() =='K' && piece->get_color() == true)
        return 11;
    else
        return -1;
}

void initTable()
{
    for (int i = 0; i<8; i++)
      for (int j = 0; j<8; j++)
        for (int k = 0; k<12; k++)
          ZobristTable[i][j][k] = randomInt();
}

void computeHash(Table* board)
{
    for (int i = 0; i<8; i++)
    {
        for (int j = 0; j<8; j++)
        {
            Piece* crt_piece = board->get_piece(i, j);
            if (board->get_piece(i, j) != NULL)
            {
                int piece = indexOf(crt_piece);
                hvalue ^= ZobristTable[i][j][piece];
            }
        }
    }
}

void updateHash(Piece* moved, int origX, int origY, int finalX, int finalY) {

    hvalue ^= ZobristTable[origX][origY][indexOf(moved)];
    hvalue ^= ZobristTable[finalX][finalY][indexOf(moved)];
}

pair<int, Move*> alphabeta_negamax(Table* state, int depth, bool player, int offset, int alpha, int beta);
vector<Move*> all_moves(Table* state, bool player, int offset);
unordered_map<char, int> values;
unordered_map<size_t, string> coords;
inline size_t key(int i,int j) {return (size_t) i << 32 | (unsigned int) j;}

//Extracts a new piece
int get_random(Table* chess_table, Piece* &piece, bool bot_color, int offset) {

    //Setting random seed
    uint32_t seed = chrono::system_clock::now()
            .time_since_epoch()
            .count();

    srand(time(NULL));
    default_random_engine eng(seed);
    vector<Piece*> availablePieces;

    //Add all the available pieces for a specific color in a vector
    for(int i = 0; i < TABLE_SIZE; i++) {
        for(int j = 0; j < TABLE_SIZE; j++) {
            if(chess_table->get_piece(i, j) != nullptr &&
                chess_table->get_piece(i, j)->get_color() == bot_color) {
                availablePieces.push_back(chess_table->get_piece(i, j));
            }
        }
    }

    //Shuffle this vector of pieces
    shuffle(availablePieces.begin(), availablePieces.end(), eng);

    //Extract the first piece, if it cannot be moved, then try to use the next
    int index = 0;
    piece = availablePieces[index];
    while(piece->can_move(chess_table, offset) == -1) {
        index++;
        if(index == (int) availablePieces.size()) {
            return -1;
        }
        piece = availablePieces[index];
    }
    return 0;
}


//Setting en_passant variable for the human-controlled pawns to false
void set_en_passant_false(Table* chess_table, bool bot_color) {
    for(int i = 0; i < TABLE_SIZE; i++) {
        for(int j = 0; j < TABLE_SIZE; j++) {
            if((chess_table->get_piece(i, j) != nullptr)
            && (chess_table->get_piece(i, j)->get_type() == 'P')
            && (chess_table->get_piece(i, j)->get_color() != bot_color)) {
                ((Pawn*) chess_table->get_piece(i, j))->set_en_passant(false);
            }
        }
    }
}

//Performs an en passant move when possible
bool move_en_passant(Table* chess_table, bool bot_color, int offset,
bool can_en_passant) {
    if(!can_en_passant) return false;

    for(int i = 0; i < TABLE_SIZE; i++) {
        for(int j = 0; j < TABLE_SIZE; j++) {
            if((chess_table->get_piece(i, j) != nullptr)
            && (chess_table->get_piece(i, j)->get_type() == 'P') &&
                (chess_table->get_piece(i, j)->get_color() == bot_color)) {

                Piece* piece1_next = chess_table->get_piece(i, j)
                ->is_in_range(i, j + 1)
                ? chess_table->get_piece(i, j + 1) : nullptr;
                
                Piece* piece2_next = chess_table->get_piece(i, j)
                ->is_in_range(i, j - 1)
                ? chess_table->get_piece(i, j - 1) : nullptr;
                
                if(!((Pawn*) chess_table->get_piece(i, j))
                ->check_next_and_move(piece1_next, chess_table, offset, 1)) {

                    if(!((Pawn*) chess_table->get_piece(i, j))
                    ->check_next_and_move(piece2_next, chess_table,
                    offset, -1)) {
                    
                        continue;
                    }
                }
                return true;
            }
        }
    }
    return false;
}

//switch the player
void switch_offset(int& offset) {
    if(offset == 1)
        offset = -1;
    else offset = 1;
}

//Initialise all the variables used
void init(Table* &chess_table,
Piece* &piece, int &offset, bool &bot_color) {
    chess_table = new Table();
    offset = 1; //white moves first
    initTable();

    bot_color = true; //Set the bot as playing with black
    get_random(chess_table, piece, bot_color, offset);
    values['-'] = 0;
    values['P'] = 5;
    values['Q'] = 30;
    values['B'] = 10;
    values['N'] = 10;
    values['R'] = 30;
    values['K'] = 1000;
    for(int lin = 0; lin < TABLE_SIZE; lin++) {
        for(int col = 0; col < TABLE_SIZE; col++) {
            char col_chr = col + 'a';
            string pos_final = to_string(lin + 1).insert(0, &col_chr, 1);
            coords[key(lin, col)] = pos_final;
        }
    }
}

//Checks if a string is a valid move
bool is_move(string input_line) {
	return (input_line.size() == 4) && (isdigit(input_line[1]) == true);
}

//Checks if a string is a valid promotion move
bool is_promotion(string input_line) {
    return (input_line.size() == 5) && (isdigit(input_line[1]) == true);
}

int main() {

	//Set the seed for obtaining randomised pieces
    srand(time(NULL));

    Table* chess_table;
    Piece* piece = nullptr; //The piece moved by the bot
    int offset;			   //Offset based on the current player
    bool bot_color;		   //The color that the bot should move
    bool start = true;     //Determines if the 'go' command has been executed
    
    /*Decides whether the last piece moved by the opponent was a
    pawn with 2 positions */
    bool can_en_passant = false;

    //Read all info from stdin line by line
    while(cin) {
        string input_line;
        getline(cin, input_line);

        if(!input_line.find("xboard")) {
            continue;
        }
        else if(!input_line.find("new")) {
     		//Initialise all the attributes necessary for a new game
            start = true;
            init(chess_table,
            piece, offset, bot_color);
            
            continue;
        }
        else if(!input_line.find("protover")) {
        	//Tell xboard the additional features wanted
            cout << "feature san=0 sigint=0\n";
            continue;
        }
        else if(!input_line.find("force")) {
        	//Set the bot in 'force' mode by pausing it
            start = false;
            continue;
        }
        else if(!input_line.find("go")) {
        	//Unpauses the bot
            start = true;
        }
        else if(!input_line.find("quit")) {
            //Successfully ends the program
            exit(0);
        }
    	
        else if (is_move(input_line)) {

        	//switch the player that moves/the offset
            switch_offset(offset);

            //get the start and initial positions of this move
            string poz_init = input_line.substr(0, 2);
            string poz_final = input_line.substr(2, 2);


            //Extracts the initial and final positions as indices
            int col_init = poz_init[0] - 'a';
            int lin_init = poz_init[1] - '1';

            int col_final = poz_final[0] - 'a';
            int lin_final = poz_final[1] - '1';

            can_en_passant = false;
            //Checks if the human player performed a castling
        	if(chess_table->get_piece(poz_init)->get_type() == 'K') {
                if(abs(col_final - col_init) == 2) {
                    char small_col = 'f'; //position after small castling
                    char big_col = 'd'; //position of tower after big castling
                    Piece* small_tower = chess_table->get_piece(lin_init, 7);
                    Piece* big_tower = chess_table->get_piece(lin_init, 0);

                    /*Perform castling on the internal table,
                    regardless of the color */
                    if(col_final == 6) {
                        chess_table->swap_tiles(small_tower->get_position(),
                        to_string(lin_init + 1).insert(0, &small_col, 1));
                    } else if(col_final == 2) {
                        chess_table->swap_tiles(big_tower->get_position(),
                        to_string(lin_init + 1).insert(0, &big_col, 1));
                    }
                }
            }
            //Checks if the human player performed en passant move
           if(chess_table->get_piece(poz_init)->get_type() == 'P') {
                // Checks if the pawn has been moved 2 positions
                if(abs(lin_init - lin_final) == 2) {
                    //Set the en passant flag to true
                    ((Pawn*) chess_table->get_piece(poz_init))
                    ->set_en_passant(true);
                    can_en_passant = true;
                }

                /*If the human has moved the pawn to a different column which
                is not occupied by a piece, then it means he has performed
                en passant */
                if(chess_table->get_piece(poz_final) == nullptr) {
                    if(col_init != col_final) {
                        chess_table->set_piece(nullptr, lin_init, col_final);
                    }
                }
            }

        	//Else move the piece on the internal representation of the table
            chess_table->swap_tiles(poz_init, poz_final);
        } else if(is_promotion(input_line)) {
            //switch the player that moves/the offset
            switch_offset(offset);

            //get the start and initial positions of this move
            string poz_init = input_line.substr(0, 2);
            string poz_final = input_line.substr(2, 2);

            //Move the piece on the internal representation of the table
            chess_table->swap_tiles(poz_init, poz_final);

            char promoted_piece = input_line[4];
            Piece* new_piece;
            //Treat all cases of a promotion
            switch(promoted_piece) {
                case 'q':
                    new_piece = new Queen(!bot_color, poz_final);
                    break;
                case 'n':
                    new_piece = new Knight(!bot_color, poz_final);
                    break;
                case 'b':
                    new_piece = new Bishop(!bot_color, poz_final);
                    break;
                case 'r':
                    new_piece = new Rook(!bot_color, poz_final);
                    break;
            }
            chess_table->set_piece(new_piece, poz_final);
        }
        else {
            continue;
        }

        //Send moves if the bot is not paused
        while (start) {
            
        	//If the bot previously played black, but we want it to play white
            if(bot_color && offset == 1) {
                bot_color = false;
            	//Then pick another random piece of the desired color
            }

            //If it played white, but we want black
            else if(!bot_color && offset == -1) {
                bot_color = true;
            }

                computeHash(chess_table);

                if(chess_table->is_in_chess(false)) {
                    chess_table->white_checks += 1;
                } if(chess_table->is_in_chess(true)) {
                    chess_table->black_checks += 1;
                }

                pair<int, Move*> my_pair = alphabeta_negamax(chess_table, DEPTH, bot_color, offset, -100000, +100000);
                if(my_pair.second!= nullptr) {
                    char slin = '1';
                    char scol = 'a';
                    slin += my_pair.second->getX();
                    scol += my_pair.second->getY();
                    char flin = slin + my_pair.second->getRelative().first;
                    char fcol = scol + my_pair.second->getRelative().second;
                    cout << "move " << scol << slin << fcol << flin << "\n";
                    int sint = slin - '1';
                    int fint = flin - '1';
                    string pos_init = to_string(sint + 1).insert(0, &scol, 1);
                    string pos_fin = to_string(fint +1).insert(0, &fcol, 1);

                    chess_table->swap_tiles(pos_init, pos_fin);
                }

            set_en_passant_false(chess_table, bot_color);

           	//Switch current player
            switch_offset(offset);
           	break;
       	}
    }
    return 0;
}

int evaluate(Table* state, bool player) {
    int sumMaterial = 0;
    int vsMaterial = 0;

    int sumPos = 0;
    int vsPos = 0;

    int crt_pos_val = 0;

    int diff = 0;

    int playerChecks = player ? state->black_checks : state->white_checks;
    int enemyChecks = player ? state->white_checks : state->black_checks;


    if(state->is_in_chess(player)) {
        switch(playerChecks) {
            case 0:
                diff = -50;
                break;
            case 1:
                diff = -100;
                break;
            case 2:
                diff = -1000;
                break;
        }

    } else if(state->is_in_chess(!player)) {
        switch(enemyChecks) {
            case 0:
                diff = 50;
                break;
            case 1:
                diff = 100;
                break;
            case 2:
                diff = 1000;
                break;
        }
    }


    for(int i = 0; i < TABLE_SIZE; i++) {
        for(int j = 0; j < TABLE_SIZE; j++) {
            if(state->get_piece(i,j) != NULL) {
                int liniarised = i * TABLE_SIZE + j;

                if(state->get_piece(i, j)->get_color() == player)
                    sumMaterial += values[state->get_piece(i, j)->get_type()];
                else
                    vsMaterial += values[state->get_piece(i, j)->get_type()];
                
                switch(state->get_piece(i, j)->get_type()) {
                    case 'P':
                        if(!state->get_piece(i, j)->get_color()) {
                            crt_pos_val = pst_pawn[liniarised];
                        } else crt_pos_val = pst_pawn[flip[liniarised]];
                        break;
                    case 'K':
                        if(!state->get_piece(i, j)->get_color()) {
                            crt_pos_val = pst_king[liniarised]; 
                        } else crt_pos_val = pst_king[flip[liniarised]];
                        break;
                    case 'Q':
                        if(!state->get_piece(i, j)->get_color()) {
                            crt_pos_val = pst_bishop[liniarised] + pst_rook[liniarised];
                        } else crt_pos_val = pst_bishop[flip[liniarised]] + pst_rook[flip[liniarised]];
                        break;
                    case 'B':
                        if(!state->get_piece(i, j)->get_color()) {
                            crt_pos_val = pst_bishop[liniarised];
                        } else crt_pos_val = pst_bishop[flip[liniarised]];
                        break;
                    case 'R':
                        if(!state->get_piece(i, j)->get_color()) {
                            crt_pos_val = pst_rook[liniarised];
                        } else crt_pos_val = pst_rook[flip[liniarised]];
                        break;
                    case 'N':
                        if(!state->get_piece(i, j)->get_color()) {
                            crt_pos_val = pst_knight[liniarised];
                        } else crt_pos_val = pst_knight[flip[liniarised]];
                        break;
                }

                if(player == state->get_piece(i, j)->get_color()) {
                    sumPos += crt_pos_val;
                } else vsPos += crt_pos_val;
            }
        }   
    }
    int h = 6 * (sumMaterial - vsMaterial) + sumPos - vsPos + diff;
    return h;
}

void apply_move(Table* state, Move* move) {
    Piece* crt_piece = state->get_piece(move->getX(), move->getY());
    string position = crt_piece->get_position();

    int col = position[0] - 'a' + move->getRelative().second;
    int lin = position[1] - '1' + move->getRelative().first;
    string pos_final = coords[key(lin, col)];

    state->eliminated.push(state->get_piece(lin, col));

    hvalue ^= ZobristTable[move->getX()][move->getY()][indexOf(crt_piece)];
    hvalue ^= ZobristTable[lin][col][indexOf(crt_piece)];
    Piece* replaced = state->get_piece(lin, col);
    if(replaced != NULL)
        hvalue ^= ZobristTable[lin][col][indexOf(replaced)];

    int v = state->swap_tiles(crt_piece->get_position(), pos_final);
}

void undo_move(Table* state, Move* move) {
    int originalX = move->getX() + move->getRelative().first;
    int originalY = move->getY() + move->getRelative().second;

    Piece* crt_piece = state->get_piece(originalX, originalY);

    string position = crt_piece->get_position();

    int col = position[0] - 'a' - move->getRelative().second;
    int lin = position[1] - '1' - move->getRelative().first;
    string pos_final = coords[key(lin, col)];

    hvalue ^= ZobristTable[originalX][originalY][indexOf(crt_piece)];
    hvalue ^= ZobristTable[lin][col][indexOf(crt_piece)];


    int v = state->swap_tiles(crt_piece->get_position(), pos_final);
    crt_piece->set_moved(false);

    Piece* old = state->eliminated.top();
    state->eliminated.pop();

    if(old != NULL) {
        old->set_moved(false);
        hvalue ^= ZobristTable[originalX][originalY][indexOf(old)];
    }

    state->set_piece(old, originalX, originalY);

}

bool game_over(Table* state) {
   return state->white_checks == 3 || state->black_checks == 3; 
}

bool get_opponent(bool player) {
    return !player;
}

bool is_in_range2(int i, int j) {
    return (i >= 0) && (j >= 0) && (i < TABLE_SIZE) && (j < TABLE_SIZE);
}

bool compareMoves(Move* m1, Move* m2) {
    
    int h1, h2;
    char piece1_type = '-', piece2_type = '-';


    int x1 = m1->getX() + m1->getRelative().first;
    int x2 = m2->getX() + m2->getRelative().first;
    int y1 = m1->getY() + m1->getRelative().second;
    int y2 = m2->getY() + m2->getRelative().second;

    Piece* piece1 = staticTable->get_piece(x1, y1);
    Piece* piece2 = staticTable->get_piece(x2, y2);
    
    if(piece1 != NULL) piece1_type = piece1->get_type();
    if(piece2 != NULL) piece2_type = piece2->get_type();

    h1 = values[piece1_type];
    h2 = values[piece2_type];

    int liniarised = x1 * TABLE_SIZE + y1;
    int pos1, pos2;
    int crt_pos_val;

    switch(staticTable->get_piece(m1->getX(), m1->getY())->get_type()) {
        case 'P':
            if(!staticTable->get_piece(m1->getX(), m1->getY())->get_color()) {
                crt_pos_val = pst_pawn[liniarised];
            } else crt_pos_val = pst_pawn[flip[liniarised]];
            break;
        case 'K':
            if(!staticTable->get_piece(m1->getX(), m1->getY())->get_color()) {
                crt_pos_val = pst_king[liniarised]; 
            } else crt_pos_val = pst_king[flip[liniarised]];
            break;
        case 'Q':
            if(!staticTable->get_piece(m1->getX(), m1->getY())->get_color()) {
                crt_pos_val = pst_bishop[liniarised] + pst_rook[liniarised];
            } else crt_pos_val = pst_bishop[flip[liniarised]] + pst_rook[flip[liniarised]];
            break;
        case 'B':
            if(!staticTable->get_piece(m1->getX(), m1->getY())->get_color()) {
                crt_pos_val = pst_bishop[liniarised];
            } else crt_pos_val = pst_bishop[flip[liniarised]];
            break;
        case 'R':
            if(!staticTable->get_piece(m1->getX(), m1->getY())->get_color()) {
                crt_pos_val = pst_rook[liniarised];
            } else crt_pos_val = pst_rook[flip[liniarised]];
            break;
        case 'N':
            if(!staticTable->get_piece(m1->getX(), m1->getY())->get_color()) {
                crt_pos_val = pst_knight[liniarised];
            } else crt_pos_val = pst_knight[flip[liniarised]];
            break;
    }

    pos1 = crt_pos_val;

    liniarised = x2 * TABLE_SIZE + y2;

    switch(staticTable->get_piece(m2->getX(), m2->getY())->get_type()) {
        case 'P':
            if(!staticTable->get_piece(m2->getX(), m2->getY())->get_color()) {
                crt_pos_val = pst_pawn[liniarised];
            } else crt_pos_val = pst_pawn[flip[liniarised]];
            break;
        case 'K':
            if(!staticTable->get_piece(m2->getX(), m2->getY())->get_color()) {
                crt_pos_val = pst_king[liniarised]; 
            } else crt_pos_val = pst_king[flip[liniarised]];
            break;
        case 'Q':
            if(!staticTable->get_piece(m2->getX(), m2->getY())->get_color()) {
                crt_pos_val = pst_bishop[liniarised] + pst_rook[liniarised];
            } else crt_pos_val = pst_bishop[flip[liniarised]] + pst_rook[flip[liniarised]];
            break;
        case 'B':
            if(!staticTable->get_piece(m2->getX(), m2->getY())->get_color()) {
                crt_pos_val = pst_bishop[liniarised];
            } else crt_pos_val = pst_bishop[flip[liniarised]];
            break;
        case 'R':
            if(!staticTable->get_piece(m2->getX(), m2->getY())->get_color()) {
                crt_pos_val = pst_rook[liniarised];
            } else crt_pos_val = pst_rook[flip[liniarised]];
            break;
        case 'N':
            if(!staticTable->get_piece(m2->getX(), m2->getY())->get_color()) {
                crt_pos_val = pst_knight[liniarised];
            } else crt_pos_val = pst_knight[flip[liniarised]];
            break;
    }

    pos2 = crt_pos_val;
    h1 += pos1;
    h2 += pos2;

    return h1 > h2;
}

vector<Move*> all_moves(Table* state, bool player, int offset) {

    state->movelist.clear();

    vector<Move*> move_list;
    for(int i = 0; i < TABLE_SIZE; i++) {
        for(int j = 0; j < TABLE_SIZE; j++) {
            Piece* crt_piece = state->get_piece(i, j);
            if(crt_piece != NULL && crt_piece->get_color() == player) {
                crt_piece->can_move(state, offset);
            } 
        }
    }

    for(int k = 0; k < state->movelist.size(); k++) {
        Piece* crt_piece = state->get_piece(state->movelist[k]->getX(), state->movelist[k]->getY());
        if(is_in_range2(state->movelist[k]->getX() + state->movelist[k]->relativeMove.first,
        state->movelist[k]->getY() + state->movelist[k]->relativeMove.second))
            if(crt_piece->get_type() != 'K' ||
            (abs(state->movelist[k]->relativeMove.first) <= 1 && abs(state->movelist[k]->relativeMove.second) <= 1 ))
                move_list.push_back(state->movelist[k]);
    }

    staticPlayer = player;
    staticTable = state;
    sort(state->movelist.begin(), state->movelist.end(), compareMoves);

    return state->movelist;
}

pair<int, Move*> alphabeta_negamax(Table* state, int depth, bool player, int offset, int alpha, int beta) {
    
    int alphaOrig = alpha;

    if(transpositionTable.find(hvalue) != transpositionTable.end()) {
        ttEntry tt = transpositionTable[hvalue];
        if(tt.depth >= depth) {
            if(tt.flag == EXACT) {
                return make_pair(tt.value, nullptr);
            } else if (tt.flag == LOWERBOUND) {
                alpha = max(alpha, tt.value);
            } else if (tt.flag == UPPERBOUND) {
                beta = min(beta, tt.value);
            }

            if(alpha >= beta) {
                return make_pair(tt.value, nullptr);
            }
        }
    }

    if(game_over(state) || depth == 0) {
        return make_pair(evaluate(state, player), nullptr);
    }

    vector<Move*> all_possible_moves = all_moves(state, player, offset);

    int best_score = -100000;
    Move* best_move = nullptr;

    for(int i = 0; i < all_possible_moves.size(); i++) {
        Move* crt_move = all_possible_moves[i];
        apply_move(state, crt_move);
        pair<int, Move*> res = alphabeta_negamax(state, depth - 1, get_opponent(player), -offset, -beta, -alpha);

        int score = -res.first;
        if(score > best_score) {
            best_score = score;
            best_move = crt_move;
        }
        if(best_score > alpha) {
            alpha = best_score;
        }
        if(alpha >= beta) {
           undo_move(state, crt_move);
           break;
        }

        undo_move(state, crt_move);
    }

    ttEntry tt;

    tt.value = best_score;
    if(best_score <= alphaOrig) {
        tt.flag = UPPERBOUND;
    } else if (best_score >= beta) {
        tt.flag = LOWERBOUND;
    } else tt.flag = EXACT;
    tt.depth = depth;
    transpositionTable[hvalue] = tt;

    return make_pair(best_score, best_move);
}
