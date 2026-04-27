#include "../h_files/PlayerInitialization.h"
#include "../h_files/Definers.h"
#include "../h_files/Player.h"
#include "../h_files/KeyboardInputHandling.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// function to determine which colours have been already taken by other players
vector<string> determineTakenColours(vector<Player*> players)
{
	vector<string> takenColours;

	// if vector of players is not empty
	if (!players.empty())
	{
		// iterate over all the players
		for (int playerIndex = 0; playerIndex < players.size(); playerIndex++)
		{
			// insert all the colours already assigned
			takenColours.push_back(players[playerIndex]->getColour());			
		}
	}

	// return the vector with all the taken colours
	return takenColours;
}

// function making it impossible to choose already taken colour
vector<char> determineAvailableColours(vector<string> takenColours)
{
	vector<char> validKeyboardInputs;

	// if there are no taken colours
	if (takenColours.empty())
	{
		// pollute the vector with all the possible options
		validKeyboardInputs.push_back(COLOUR_BLACK_KEYCAP);
		validKeyboardInputs.push_back(COLOUR_RED_KEYCAP);

		// display full Menu for choosing the colour
		cout << PLAYER_CHOOSE_COLOUR_BLACK_AVAILABLE_DISPLAY;
		cout << PLAYER_CHOOSE_COLOUR_RED_AVAILABLE_DISPLAY;
		// display end of menu for choosing the colour
		cout << PLAYER_CHOOSE_COLOUR_FOOTER_DISPLAY;
	}
	// if one of the colours is taken already
	else
	{
		// there is maximum one colour taken
		string takenColour = takenColours[PLAYER_CHOOSE_COLOUR_TAKEN_COLOUR_INDEX];

		// determine which colour has been taken and display the menu accordingly
		if (takenColour == COLOUR_BLACK_NAME)
		{
			validKeyboardInputs.push_back(COLOUR_RED_KEYCAP);

			// display the Menu for choosing the colour with BLACK unavailable
			cout << PLAYER_CHOOSE_COLOUR_BLACK_UNAVAILABLE_DISPLAY;
			cout << PLAYER_CHOOSE_COLOUR_RED_AVAILABLE_DISPLAY;
			// display end of menu for choosing the colour
			cout << PLAYER_CHOOSE_COLOUR_FOOTER_DISPLAY;
		}
		else
		{
			validKeyboardInputs.push_back(COLOUR_BLACK_KEYCAP);

			// display the Menu for choosing the colour with RED unavailable
			cout << PLAYER_CHOOSE_COLOUR_BLACK_AVAILABLE_DISPLAY;
			cout << PLAYER_CHOOSE_COLOUR_RED_UNAVAILABLE_DISPLAY;
			// display end of menu for choosing the colour
			cout << PLAYER_CHOOSE_COLOUR_FOOTER_DISPLAY;
		}
	}

	return validKeyboardInputs;
}

// function to set the name of the player from the console
void setNameForThePlayer (Player* player)
{
	// display Menu for changing the name
	cout << PLAYER_NAME_CHANGE_CHANGE_THE_NAME_DISPLAY;
	cout << PLAYER_NAME_CHANGE_DO_NOT_CHANGE_THE_NAME_DISPLAY;
	// display end of menu for choosing whether to change the name or not
	cout << PLAYER_NAME_CHANGE_FOOTER_DISPLAY;

	// make it impossible for the player to provide invalid input
	vector<char> validKeyboardInputs;

	validKeyboardInputs.push_back(PLAYER_NAME_CHANGE_CHANGE_THE_NAME_KEYCAP);
	validKeyboardInputs.push_back(PLAYER_NAME_CHANGE_DO_NOT_CHANGE_THE_NAME_KEYCAP);

	char validInput = handleKeyboardInput(validKeyboardInputs);

	// if player wants to change the name
	if (validInput == PLAYER_NAME_CHANGE_CHANGE_THE_NAME_KEYCAP)
	{
		string name;

		// repeat until the name is of correct length
		do {
			cout << PLAYER_NAME_CHANGE_SET_NAME_DISPLAY;
			cin >> name;

			// if the name is too long display the reminder
			if (name.size() > PLAYER_NAME_CHANGE_NAME_MAX_LENGTH)
			{
				cout << PLAYER_NAME_CHANGE_INVALID_NAME_LENGTH_DISPLAY;
			}
		}
		while (name.size() > PLAYER_NAME_CHANGE_NAME_MAX_LENGTH);

		// otherwise set the name and display it
		player->setName(name);
		cout << PLAYER_NAME_CHANGE_NEW_NAME_DISPLAY << player->getName() << PLAYER_NAME_CHANGE_SET_NAME_FOOTER_DISPLAY;
	}
	// if the player doesn't want to change the name and use default one instead
	else
	{
		cout << PLAYER_NAME_CHANGE_DEFAULT_NAME_DISPLAY << player->getName() << PLAYER_NAME_CHANGE_SET_NAME_FOOTER_DISPLAY;
	}
}

// function to initialize players and their attributes
vector<Player*> initPlayers()
{
	vector<Player*> players;

	// initialize 'PLAYER_MAX_NUMBER_PER_GAME' players for a single game
	for (int playerIndex = 0; playerIndex < PLAYER_MAX_NUMBER_PER_GAME; playerIndex++)
	{
		cout << PLAYER_CHOOSE_COLOUR_PLAYER_DISPLAY << playerIndex + PLAYER_ID_INCREMENT << PLAYER_CHOOSE_COLOUR_DISPLAY;

		// determine colours which are not available
		vector<string> takenColours = determineTakenColours(players);

		// determine which kind of inputs are valid (only available colours)
		vector<char> validKeyboardInputs = determineAvailableColours(takenColours);

		// handle all the keyboard inputs
		char validInput = handleKeyboardInput(validKeyboardInputs);
		string colour;

		// assign colours accordingly
		if (validInput == COLOUR_BLACK_KEYCAP)
		{
			colour = COLOUR_BLACK_NAME;
		}
		else
		{
			colour = COLOUR_RED_NAME;
		}

		// create an instance of a player, the 'id' has to be incremented to represent a real value
		Player* player = new Player(playerIndex + PLAYER_ID_INCREMENT, colour);

		// setting name possibility for the player
		setNameForThePlayer(player);

		// add the player to 'players' vector holding all the players
		players.push_back(player);
	}

	return players;
}
