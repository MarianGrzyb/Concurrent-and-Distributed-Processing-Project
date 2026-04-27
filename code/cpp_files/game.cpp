#include "../h_files/Definers.h"
#include "../h_files/Player.h"
#include "../h_files/Field.h"
#include "../h_files/KeyboardInputHandling.h"
#include "../h_files/MainMenu.h"
#include "../h_files/PlayerInitialization.h"
#include "../h_files/FieldsInitialization.h"
#include "../h_files/Board.h"

#include "../h_files/server.h"
#include "../h_files/socket.h"

#include <iostream>
#include <vector>
#include <windows.h>

using namespace std;

// function to determine who moves first and who second
void determinePlayerOrder(vector<Player*> players, Player* firstPlayer, Player* secondPlayer)
{
	// if the first player chooses 'BLACK_COLOUR_NAME' then they start
	if (players[0]->getColour() == COLOUR_BLACK_NAME)
	{
		*firstPlayer = *players[0];
		*secondPlayer = *players[1];
	}
	// otherwise second player starts
	else
	{
		*firstPlayer = *players[1];
		*secondPlayer = *players[0];
	}
}

// Prints available columns and fills validKeyboardInputs. Returns number of available columns found.
int determinePossibleMoves(vector<Field*> allFields, vector<char>& validKeyboardInputs)
{
	int availableColumns = 0;
	validKeyboardInputs.push_back(TURN_QUIT_TO_MAIN_MENU_KEYCAP);

	// iterate through all the Fields
	for (int fieldIndex = 0; fieldIndex < allFields.size(); fieldIndex++)
	{
		Field field = *allFields[fieldIndex];

		// if the highest row is unoccupied
		if (field.getRow() == ROWS_NUMBER && !field.getOccupied())
		{
			availableColumns++;
			cout << TURN_AVAILABLE_COLUMN_DISPLAY << field.getColumn() << TURN_AVAILABLE_COLUMN_FOOTER_DISPLAY;
			validKeyboardInputs.push_back(char(CASTING_TO_CHAR + field.getColumn()));
		}
	}

	// if there is at least one possible move
	if (availableColumns != 0)
	{
		cout << TURN_QUIT_TO_MAIN_MENU_DISPLAY;
	}

	return availableColumns;
}

// function to quit to Main Menu
bool gameQuitting(char validInput, bool* quitGame, Player currentPlayer)
{
	// if the option to exit to Main Menu is chosen
	if (validInput == TURN_QUIT_TO_MAIN_MENU_KEYCAP)
	{
		// end the game and quit to Main Menu
		*quitGame = true;
		cout << TURN_QUIT_TO_MAIN_MENU_PLAYER_DISPLAY << currentPlayer.getName() << TURN_QUIT_TO_MAIN_MENU_DESCRIPTION_DISPLAY;
		return true;
	}

	return false;
}

// Places a pawn in the lowest unoccupied row of the chosen column
void pawnPlacing(vector<Field*>& allFields, char symbol, int columnInput)
{
	int lowestRow = ROWS_NUMBER;
	int lowestRowIndex = 0;

	// iterate through all the Fields
	for (int fieldIndex = 0; fieldIndex < allFields.size(); fieldIndex++)
	{
		Field field = *allFields[fieldIndex];

		// if the column matches the input and the row is the lowest unoccupied
		if (field.getColumn() == columnInput && field.getRow() <= lowestRow && !field.getOccupied())
		{
			lowestRow = field.getRow();
			lowestRowIndex = fieldIndex;
		}
	}

	// place the pawn in the correct Field
	allFields[lowestRowIndex]->setOccupied(true);
	allFields[lowestRowIndex]->setSymbol(symbol);
}

// function to handle the course of a turn
void turn(Player currentPlayer, vector<Field*>& allFields, bool* quitGame)
{
	// display all the information about the turn
	cout << TURN_COLOUR_DISPLAY << currentPlayer.getColour() << TURN_PLAYER_DISPLAY << currentPlayer.getName() << TURN_FOOTER_DISPLAY;
	cout << TURN_POSSIBLE_MOVES_DISPLAY;

	vector<char> validKeyboardInputs;
	int availableColumns = determinePossibleMoves(allFields, validKeyboardInputs);

	// if there are no more columns available
	if (availableColumns == 0)
	{
		// finish the game with a draw
		*quitGame = true;
		cout << DRAW_EVENT_DESCRIPTION_DISPLAY;
		return;
	}

	char validInput = handleKeyboardInput(validKeyboardInputs);

	// check whether the player decided to exit to Main Menu
	if (gameQuitting(validInput, quitGame, currentPlayer))
	{
		return;
	}

	int columnInput = int(validInput - CASTING_TO_CHAR);

	// place the pawns accordingly
	pawnPlacing(allFields, currentPlayer.getFieldSymbol(), columnInput);
}

// help function to check if Field exists on the given position
Field* fieldAt(const vector<Field*>& allFields, int row, int col)
{
	if (row < 1 || row > ROWS_NUMBER || col < 1 || col > COLUMNS_NUMBER)
	{
		return nullptr;
	}

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
		{
			continue;
		}

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
			{
				return true;
			}
		}
	}

	return false;
}

// function to carry out the game
void mainGameLoop(vector<Player*> players, HANDLE consoleColour)
{
	// initialize all the Fields possible on the board
	vector<Field*> allFields = initFields();

	Player firstPlayer;
	Player secondPlayer;
	Player currentPlayer;

	// determine which player starts
	determinePlayerOrder(players, &firstPlayer, &secondPlayer);

	// count the turns
	int turnCounter = 0;
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
		turn(currentPlayer, allFields, &quitGame);

		if (victory(allFields, currentPlayer.getFieldSymbol()))
		{
			displayBoard(allFields);

			//display the text in colour
			SetConsoleTextAttribute(consoleColour, TEXT_COLOUR_GREEN);
			cout << WIN_EVENT_PLAYER_DISPLAY << currentPlayer.getName() << WIN_EVENT_COLOUR_DISPLAY << currentPlayer.getColour() << WIN_EVENT_FOOTER_DISPLAY;
			SetConsoleTextAttribute(consoleColour, TEXT_COLOUR_DEFAULT);

			quitGame = true;
			return;
		}
	}
	// finish the game when one of the players wins or there is a draw (no more possible moves)
	while (!quitGame);
}

// preparatory function to start the game
void startNewGame(HANDLE consoleColour)
{
	// initialize players
	vector<Player*> players = initPlayers();

	// start the main game loop for specific players
	mainGameLoop(players, consoleColour);
}

// the Main game interface
int main()
{
	//display the text in colour
	HANDLE consoleColour = GetStdHandle(STD_OUTPUT_HANDLE);

	connectToServer();

	do
	{
		// choose current option in the Main Menu
		char currentOption = mainMenu();

		// if option 'MAIN_MENU_START_NEW_GAME_KEYCAP' then start the game
		if (currentOption == MAIN_MENU_START_NEW_GAME_KEYCAP)
		{
			startNewGame(consoleColour);
		}
		// if option 'QUIT_THE_GAME_KEYCAP' then quit the game
		else if (currentOption == MAIN_MENU_QUIT_THE_GAME_KEYCAP)
		{
			break;
		}
	}
	while (true);
}