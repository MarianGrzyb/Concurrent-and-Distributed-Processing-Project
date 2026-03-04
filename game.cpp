#include "Definers.h"
#include "Player.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// function to iterate over the vector of all possible inputs 'inputCharacters' to determine whether a given character stream
// 'characterStream' is included in this vector
bool checkKeyboardInputCorrectness(vector<char> inputCharacters, string characterStream)
{
	// check whether the vector of all characters is not empty and whether the size of character stream is equal 'OPTION_SIZE'
	if (!inputCharacters.empty() && characterStream.size() == OPTION_SIZE)
	{
		// iterate over the whole vector of possible inputs 
		for (int i = 0; i < inputCharacters.size(); i++)
		{
			// if a given character has a match in the vector of all possible inputs return 'CORRECT_INPUT'
			if (characterStream[0] == inputCharacters[i])
			{
				return CORRECT_INPUT;
			}
		}
	}

	// if after iteration over the whole vector no match has been found return 'INCORRECT_INPUT'
	return INCORRECT_INPUT;
}

//function to handle all the inputs from the keyboard (in any situation)
string HandleKeyboardInput(vector<char> inputCharacters)
{
	bool result;
	string characterStream;

	do
	{
		// read character Stream from the console
		cin >> characterStream;

		// check whether the input from the console is correct
		result = checkKeyboardInputCorrectness(inputCharacters, characterStream);

		// if the input is incorrect display 'INCORRECT_INPUT_DISPLAY'
		if (result == INCORRECT_INPUT)
		{
			cout << INCORRECT_INPUT_DISPLAY;
		}
	}
	// repeat the process until input is correct
	while (result != CORRECT_INPUT);

	return characterStream;
}

// function to pint the Main Menu
void printMainMenu()
{
	cout << MAIN_MENU_START_NEW_GAME_DISPLAY << endl;
}

// function to handle the Main Menu (all the possible options)
string Menu()
{
	// print all the options from the Main Menu
	printMainMenu();

	// add all the possible valid options to 'keyboardInputs'
	vector<char> validKeyboardInputs;
	validKeyboardInputs.push_back(MAIN_MENU_START_NEW_GAME_KEYCAP);

	// handle the input from the console
	string validInput = HandleKeyboardInput(validKeyboardInputs);

	return validInput;
}


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
	// if one of the coulrs is taken already
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
		{
			colour = BLACK_COLOUR_NAME;
		}
		else
		{
			colour = RED_COLOUR_NAME;
		}

		// create an instance of a player
		Player* player = new Player(i + 1, colour);
		// insert the player to 'players' vector holding all the players
		players.push_back(player);
	}

	return players;
}

void startNewGame()
{
	// initialize players
	vector<Player*> players = initPlayers();

	for (int i = 0; i < players.size(); i++)
	{
		cout << "\nID:" << players[i]->getId() << endl;
		cout << "Name:" << players[i]->getName() << endl;
		cout << "Colour:" << players[i]->getColour() << "\n" << endl;
	}
}

int main()
{
	string option;
	// display the Menu and choose the option
	option = Menu();

	// if option 'MAIN_MENU_START_NEW_GAME_KEYCAP' then start the game
	if (option[0] == MAIN_MENU_START_NEW_GAME_KEYCAP)
	{
		startNewGame();
	}
}