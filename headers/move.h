#include <string>
#include <vector>

using namespace std;

#ifndef MOVE_H
#define MOVE_H

#define TABLE_SIZE 8

class Table;

class Move {
    public:
        int posX, posY; // pozitiile piesei de pe tabla
        pair<int, int> relativeMove; //offset ul

    public:
        
        //Constructors
        Move(int posX, int posY, pair<int, int> relativeMove);
        
        //Getters and setters
        int getX();
        int getY();
        pair<int, int> getRelative();
};

#endif