#pragma once

#include <vector>

using namespace std;

struct Coordinates
{
    int x;
    int y;
};

class Field
{
private:
    bool occupied;
    char symbol;
    int row;
    int column;
    vector <Coordinates> allCoordinates;

public:
    Field();
    Field(int row, int column, vector <Coordinates> allCoordinates);

    bool getOccupied();
    void setOccupied(bool occupied);
    char getSymbol();
    void setSymbol(char symbol);
    int getRow();
    void setRow(int row);
    int getColumn();
    void setColumn(int column);
    vector <Coordinates> getAllCooridnates();
    void setAllCoordinates(vector <Coordinates> allCoordinates);
};
