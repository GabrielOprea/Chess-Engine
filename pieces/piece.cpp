#include "../headers/piece.h"
#include <string>

Piece::Piece() {
    this->color = false;
    this->type = '\0';
    this->position = "";
    this->moved = false;
}

Piece::Piece(bool color, char type, string position) {
    this->color = color;
    this->type = type;
    this->position = position;
    this->moved = false;
}

bool Piece::get_color() {
    return color;
}
char Piece::get_type() {
    return type; 
}

void Piece::set_type(char type) {
    this->type = type;
}

string Piece::get_position() {
	return position;
}

void Piece::set_position(string position) {
	this->position = position;
}

//Checks if specific coordinates are within the table's range
bool Piece::is_in_range(int i, int j) {
    return (i >= 0) && (j >= 0) && (i < TABLE_SIZE) && (j < TABLE_SIZE);
}

void Piece::set_moved(bool flag) {
    moved = flag;
}

bool Piece::get_moved() {
    return moved;
}