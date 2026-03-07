#include "Player.h"
#include "Definers.h"

string setDefaultName(int id)
{
    if (id == DEFAULT_ID_PLAYER_1)
    {
        return DEFAULT_NAME_PLAYER_1;
    }
    else
    {
        return DEFAULT_NAME_PLAYER_2;
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