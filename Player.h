#pragma once

#include <string>

using namespace std;

class Player
{
private:
    int id;
    string name;
    string colour;
    char fieldSymbol;

public:
    Player();
    Player(int id, string colour);

    int getId();
    void setId(int id);
    string getName();
    void setName(string name);
    string getColour();
    void setColour(string colour);
    char getFieldSymbol();
    void setFieldSymbol(char fieldSymbol);
};
