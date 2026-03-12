#include "Player.h"
#include "Definers.h"

//function to set default name based on the order (id) of players
string setDefaultName(int id)
{
    if (id == PLAYER_DEFAULT_ID_1)
    {
        return PLAYER_DEFAULT_NAME_1;
    }
    else
    {
        return PLAYER_DEFAULT_NAME_2;
    }
}

Player::Player() {}

Player::Player(int id, string colour)
{
    this->id = id;
    this->name = setDefaultName(id);
    this->colour = colour;
}

int Player::getId()
{
    return this->id;
}

void Player::setId(int id)
{
    this->id = id;
}

string Player::getName()
{
    return this->name;
}

void Player::setName(string name)
{
    this->name = name;
}

string Player::getColour()
{
    return this->colour;
}

void Player::setColour(string colour)
{
    this->colour = colour;
}
