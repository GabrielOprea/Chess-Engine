int evaluate(Table* state, bool player);
void apply_move(Table* state, Move* move);
void undo_move(Table* state, Move* move);
bool game_over(Table* state);
bool get_opponent(bool player);
bool is_in_range2(int i, int j);
bool compareMoves(Move* m1, Move* m2);
vector<Move*> all_moves(Table* state, bool player, int offset);