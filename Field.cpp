#include "Field.h"
#include "Definers.h"

Field::Field() {}

Field::Field(int row, int column, vector<Coordinates> allCoordinates)
{
    this->occupied = false;
    this->symbol = FIELD_UNOCCUPIED_SYMBOL;
    this->row = row;
    this->column = column;
    this->allCoordinates = allCoordinates;
}

bool Field::getOccupied()
{
    return this->occupied;
}

void Field::setOccupied(bool occupied)
{
    this->occupied = occupied;
}

char Field::getSymbol()
{
    return this->symbol;
}

void Field::setSymbol(char symbol)
{
    this->symbol = symbol;
}

int Field::getRow()
{
    return this->row;
}

void Field::setRow(int row)
{
    this->row = row;
}

int Field::getColumn()
{
    return this->column;
}

void Field::setColumn(int column)
{
    this->column = column;
}

vector <Coordinates> Field::getAllCooridnates()
{
    return this->allCoordinates;
}

void Field::setAllCoordinates(vector <Coordinates> allCoordinates)
{
    this->allCoordinates = allCoordinates;
}