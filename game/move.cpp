#include "../headers/move.h"
#include <string>

Move::Move(int posX, int posY, pair<int, int> relativeMove) {
    this->posX = posX;
    this->posY = posY;
    this->relativeMove = relativeMove;
}

int Move::getX() {
    return posX;
}

int Move::getY() {
    return posY;
}

pair<int, int> Move::getRelative() {
    return relativeMove;
}