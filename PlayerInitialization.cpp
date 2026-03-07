#include "PlayerInitialization.h"
#include "Definers.h"
#include "Player.h"
#include "KeyboardInputHandling.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// function to determine which colours have been taken already by other players
vector<string> determineTakenColours(vector<Player*> players)
{
	vector<string> takenColours;

	// iterate over all the players
	for (int i = 0; i < players.size(); i++)
	{
		// if vector of players is not empty insert all the colours assigned to them to the vector 'takenColours'
		if (!players.empty())
		{
			takenColours.push_back(players[i]->getColour());
		}
	}

	// return the vector with all taken colours
	return takenColours;
}

// function making it impossible to choose already taken colour
vector<char> enableUniqueColours(vector<string> takenColours)
{
	vector<char> validKeyboardInputs;

	// if there are no taken colours
	if (takenColours.empty())
	{
		// pollute the vector with all the possible options
		validKeyboardInputs.push_back(BLACK_COLOUR_KEYCAP);
		validKeyboardInputs.push_back(RED_COLOUR_KEYCAP);

		// display full Menu
		cout << "BLACK[b] - STARTS THE GAME" << endl;
		cout << "RED[r] - MOVES SECOND\n" << endl;
	}
	// if one of the colours is taken already
	else
	{
		string takenColour = takenColours[0];

		// determine which colour has been taken and display the menu accordingly
		if (takenColour == BLACK_COLOUR_NAME)
		{
			validKeyboardInputs.push_back(RED_COLOUR_KEYCAP);

			cout << "BLACK - STARTS THE GAME (UNAVAILABLE)" << endl;
			cout << "RED[r] - MOVES SECOND\n" << endl;
		}
		else
		{
			validKeyboardInputs.push_back(BLACK_COLOUR_KEYCAP);

			cout << "BLACK[b] - STARTS THE GAME" << endl;
			cout << "RED - MOVES SECOND (UNAVAILABLE)\n" << endl;
		}
	}

	return validKeyboardInputs;
}

void setNameForThePlayer (Player* player) {
	cout << "Would you like to set own name for the player?" << endl;
	cout << "If yes, press 'y' else press 'n'" << endl;

	char tmp;
	cin >> tmp;

	if (tmp == 'y') {
		string name = "";
		cout << "Provide name for the player:" << endl;
		cin >> name;
		player->setName(name);
	}
	else
		cout << "Default name was assigned: " << player->getName();
}

// function to initialize players and their attributes
vector<Player*> initPlayers()
{
	vector<Player*> players;

	// initialize just 'MAX_PLAYER_NUMBER_SINGLE_GAME' players
	for (int i = 0; i < MAX_PLAYER_NUMBER_SINGLE_GAME; i++)
	{
		cout << "\nPLAYER " << i + 1 << ": CHOOSE THE COLOUR" << endl;

		// determine colours which are not available
		vector<string> takenColours = determineTakenColours(players);

		// determine which kind of inputs are valid
		vector<char> validKeyboardInputs = enableUniqueColours(takenColours);

		// handle all the keyboard inputs
		string validInput = HandleKeyboardInput(validKeyboardInputs);
		string colour;

		// assign colours accordingly
		if (validInput[0] == BLACK_COLOUR_KEYCAP)
			colour = BLACK_COLOUR_NAME;
		else
			colour = RED_COLOUR_NAME;

		// create an instance of a player
		Player* player = new Player(i + 1, colour);

		// setting name possibility for the player
		setNameForThePlayer(player);

		// add the player to 'players' vector holding all the players
		players.push_back(player);
	}

	return players;
}