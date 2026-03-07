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

// function to quit the game without ending to the end, by pressing 'q'
bool gameQuitting(const string& validInput, bool* quitGame)
{
	if (validInput == "q")
	{
		cout << "End of the game" << endl;
		*quitGame = true;
		return true;
	}
	return false;
}

// help function to check if filed exists on the given position
Field* fieldAt(const vector<Field*>& allFields, int row, int col)
{
	if (row < 1 || row > ROWS_NUMBER || col < 1 || col > COLUMNS_NUMBER)
		return nullptr;
	return allFields[(row - 1) * COLUMNS_NUMBER + (col - 1)];
}

// function to determine victory conditions
bool victory(vector<Field*>& allFields, char symbol)
{
	int directions[4][2] = {
		{0, 1},   // horizontal (on the right)
		{1, 0},   // vertical (on the top)
		{1, 1},   // diagonal upper right
		{1, -1}   // diagonal upper left
	};

	for (int i = 0; i < allFields.size(); i++)
	{
		Field* origin = allFields[i];

		if (!origin->getOccupied() || origin->getSymbol() != symbol)
			continue;

		int originRow = origin->getRow();
		int originCol = origin->getColumn();

		for (int d = 0; d < 4; d++)
		{
			int dRow = directions[d][0];
			int dCol = directions[d][1];
			int count = 1;

			for (int step = 1; step < 4; step++)
			{
				Field* candidate = fieldAt(allFields, originRow + step * dRow, originCol + step * dCol);

				if (!candidate || !candidate->getOccupied() || candidate->getSymbol() != symbol)
					break;

				count++;
			}

			if (count == 4)
				return true;
		}
	}
	return false;
}

// function to carry out a turn for a particular player
void turn(Player currentPlayer, vector<Field*> allFields, bool* poisonPill, bool* quitGame)
{
	cout << "Turn of the " << currentPlayer.getColour() << " player: <" << currentPlayer.getName() << ">" << endl;
	cout << "Possible moves: " << endl;

	int poisonPillCounter = 0;
	vector<char> validKeyboardInputs;

	validKeyboardInputs.push_back(char('q'));

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

		// check if player wants to quit game
		if (gameQuitting(validInput, quitGame))
			return;

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
	bool quitGame = false;

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
		turn(currentPlayer, allFields, &poisonPill, &quitGame);

		char currentSymbol;

		if (currentPlayer.getColour() == BLACK_COLOUR_NAME)
			currentSymbol = FIELD_OCCUPIED_SYMBOL_BLACK;
		else
			currentSymbol = FIELD_OCCUPIED_SYMBOL_RED;

		if (victory(allFields,currentSymbol)) {
			displayBoard(allFields);
			cout << currentPlayer.getColour() << ", symbol: " << currentSymbol << " wins!" << endl;
			quitGame = true;
			return;
		}
	}
	// finish the game when one of the players wins
	while (!poisonPill && !quitGame);
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