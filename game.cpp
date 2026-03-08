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
bool gameQuitting(const string& validInput, bool* quitGame, Player currentPlayer)
{
	if (validInput[0] == QUIT_THE_GAME_KEYCAP)
	{
		cout << "\nPLAYER <" <<currentPlayer.getName() << "> QUIT THE GAME\n" << endl;
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
// Prints available columns and fills validKeyboardInputs. Returns number of available columns found.
int determinePossibleMoves(vector<Field*> allFields, vector<char>& validKeyboardInputs)
{
	int availableColumns = 0;
	validKeyboardInputs.push_back(QUIT_THE_GAME_KEYCAP);

	for (int i = 0; i < allFields.size(); i++)
	{
		Field field = *allFields[i];

		if (field.getRow() == ROWS_NUMBER && !field.getOccupied())
		{
			availableColumns++;
			cout << "COLUMN [" << field.getColumn() << "]" << endl;
			validKeyboardInputs.push_back(char(CASTING_TO_CHAR + field.getColumn()));
		}
	}

	cout << "QUIT [" << QUIT_THE_GAME_KEYCAP << "]\n" << endl;

	return availableColumns;
}

// Places a pawn in the lowest unoccupied row of the chosen column.
void pawnPlacing(vector<Field*>& allFields, char symbol, int columnInput)
{
	int lowestRow = ROWS_NUMBER;
	int lowestRowIndex = 0;

	for (int i = 0; i < allFields.size(); i++)
	{
		Field field = *allFields[i];

		if (field.getColumn() == columnInput && field.getRow() <= lowestRow && !field.getOccupied())
		{
			lowestRow = field.getRow();
			lowestRowIndex = i;
		}
	}

	allFields[lowestRowIndex]->setOccupied(true);
	allFields[lowestRowIndex]->setSymbol(symbol);
}

void turn(Player currentPlayer, vector<Field*>& allFields, bool* poisonPill, bool* quitGame)
{
	cout << "TURN OF THE (" << currentPlayer.getColour() << ") PLAYER <" << currentPlayer.getName() << ">" << endl;
	cout << "POSSIBLE MOVES: " << endl;

	vector<char> validKeyboardInputs;
	int availableColumns = determinePossibleMoves(allFields, validKeyboardInputs);

	if (availableColumns == 0)
	{
		*poisonPill = true;
		cout << "NO MOVES AVAILABLE!\n" << endl;
		cout << DRAW_EVENT_DESCRIPTION << endl;
		return;
	}

	string validInput = HandleKeyboardInput(validKeyboardInputs);

	if (gameQuitting(validInput, quitGame, currentPlayer))
		return;

	int columnInput = int(validInput[0] - CASTING_TO_CHAR);
	char symbol;

	if (currentPlayer.getColour() == BLACK_COLOUR_NAME)
		symbol = FIELD_OCCUPIED_SYMBOL_BLACK;
	else
		symbol = FIELD_OCCUPIED_SYMBOL_RED;

	pawnPlacing(allFields, symbol, columnInput);
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
			currentPlayer = secondPlayer;
		else
			currentPlayer = firstPlayer;

		// display the whole state of a board
		displayBoard(allFields);
		// allow a player to take their turn
		turn(currentPlayer, allFields, &poisonPill, &quitGame);

		char currentSymbol;

		if (currentPlayer.getColour() == BLACK_COLOUR_NAME)
			currentSymbol = FIELD_OCCUPIED_SYMBOL_BLACK;
		else
			currentSymbol = FIELD_OCCUPIED_SYMBOL_RED;

		if (victory(allFields, currentSymbol))
		{
			displayBoard(allFields);
			cout << "PLAYER <" << currentPlayer.getName() << "> WHO PLAYED (" << currentPlayer.getColour() <<")" << " WINS!\n" << endl;
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
	string option;
	do
	{
		// choose action in the Main Menu
		string option = mainMenu();

		// if option 'MAIN_MENU_START_NEW_GAME_KEYCAP' then start the game
		if (option[0] == MAIN_MENU_START_NEW_GAME_KEYCAP)
		{
			startNewGame();
		}
		else if (option[0] == QUIT_THE_GAME_KEYCAP)
		{
			break;
		}
	}
	while (true);

}