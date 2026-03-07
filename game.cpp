#include "Definers.h"
#include "Player.h"
#include "Field.h"
#include "KeyboardInputHandling.h"
#include "MainMenu.h"
#include "PlayerInitialization.h"
#include "FieldsInitialization.h"
#include "Board.h"

#include <iostream>
#include <vector>
#include <string>

using namespace std;

// function to carry out a turn for a particular player
void turn(Player currentPlayer, vector<Field*> allFields, bool* poisonPill)
{
	cout << "Turn of the " << currentPlayer.getColour() << " player: <" << currentPlayer.getName() << ">" << endl;
	cout << "Possible moves: " << endl;

	vector<char> validKeyboardInputs;
	int poisonPillCounter = 0;

	// iterate through all the Fields
	for (int i = 0; i < allFields.size(); i++)
	{
		Field field = *allFields[i];
		int row = field.getRow();

		// Check whether the highest row is empty
		if (row == ROWS_NUMBER && !field.getOccupied())
		{
			//disable poison pill
			poisonPillCounter += 1;
			// display all the possible moves
			cout << "Column: [" << field.getColumn() << "]" << endl;
			// pass column numbers as valid keyboard inputs, but cast them to characters first
			validKeyboardInputs.push_back(char(CASTING_TO_CHAR + field.getColumn()));
		}
	}

	// if there are possible moves continue
	if (poisonPillCounter > 0)
	{
		string validInput = HandleKeyboardInput(validKeyboardInputs);
		int columnInput = int(validInput[0] - CASTING_TO_CHAR);

		int lowestRow = ROWS_NUMBER;
		int lowestRowIndex = 0;

		// iterate through all the Fields
		for (int i = 0; i < allFields.size(); i++)
		{
			Field field = *allFields[i];

			// if the column matches, there is a lower row and it is unoccupied
			if (field.getColumn() == columnInput && field.getRow() <= lowestRow && !field.getOccupied())
			{
				// set lowest row to this row and determine its index
				lowestRow = field.getRow();
				lowestRowIndex = i;
			}
		}

		// set the determined Field to occupied
		allFields[lowestRowIndex]->setOccupied(true);

		// if BLACK player then display black pawns
		if (currentPlayer.getColour() == BLACK_COLOUR_NAME)
		{
			allFields[lowestRowIndex]->setSymbol(FIELD_OCCUPIED_SYMBOL_BLACK);
		}
		// otherwise display RED pawns
		else
		{
			allFields[lowestRowIndex]->setSymbol(FIELD_OCCUPIED_SYMBOL_RED);
		}
	}
	// if there are no valid moves then finish with draw
	else
	{
		*poisonPill = true;
		cout << "No Moves Available!" << endl;
		cout << DRAW_EVENT_DESCRIPTION << endl;
	}
}

// function to determine victory conditions, not implemented yet
bool victory()
{
	return 0;
}

// function to carry out the game
void mainGameLoop(vector<Player*> players)
{
	// initialize all the Fields possible on the board
	vector<Field*> allFields = initFields();

	Player firstPlayer;
	Player secondPlayer;
	Player currentPlayer;

	// if the first player chooses 'BLACK_COLOUR_NAME' then they start
	if (players[0]->getColour() == BLACK_COLOUR_NAME)
	{
		firstPlayer = *players[0];
		secondPlayer = *players[1];
	}
	// otherwise second player starts
	else
	{
		firstPlayer = *players[1];
		secondPlayer = *players[0];
	}

	// count the turns
	int turnCounter = 0;
	bool poisonPill = false;

	// start the main game loop
	do
	{
		// increase the turn counter
		turnCounter += 1;

		// all the even turns are of the BLACK player and odd of the RED player
		if (turnCounter % TURN_DETERMINANT == 0)
		{
			currentPlayer = secondPlayer;
		}
		else
		{
			currentPlayer = firstPlayer;
		}

		// display the whole state of a board
		displayBoard(allFields);
		// allow a player to take their turn
		turn(currentPlayer, allFields, &poisonPill);
	}
	// finish the game when one of the players wins
	while (!victory() && !poisonPill);
}

// preparatory function to start the game
void startNewGame()
{
	// initialize players
	vector<Player*> players = initPlayers();

	// start the main game loop
	mainGameLoop(players);
}

// the Main game interface
int main()
{
	// choose action in the Main Menu
	string option = mainMenu();

	// if option 'MAIN_MENU_START_NEW_GAME_KEYCAP' then start the game
	if (option[0] == MAIN_MENU_START_NEW_GAME_KEYCAP)
	{
		startNewGame();
	}
}