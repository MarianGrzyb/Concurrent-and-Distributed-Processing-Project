#include "Board.h"
#include "Definers.h"

#include <iostream>
#include <vector>

using namespace std;

// function to display the bottom display of the board
void displayBottomDisplay(int x, int columnNumberDisplay, int* iterator)
{
	// if x in the middle of the column
	if (x % BOARD_COLUMN_WIDTH == BOARD_COLUMN_CENTER_INDEX)
	{
		// display column numbers
		cout << columnNumberDisplay;
		*iterator += 1;
	}
	// if x on the edge
	else if (x % BOARD_COLUMN_WIDTH == BOARD_COLUMN_EDGE_INDEX)
	{
		cout << BOARD_COLUMN_CORNER_SYMBOL;
	}
	// if x to the left and right from the displayed column number
	else if (x % BOARD_COLUMN_WIDTH == BOARD_COLUMN_CENTER_INDEX - BOARD_COLUMN_NUMBER_BUFFER_SIZE
		|| x % BOARD_COLUMN_WIDTH == BOARD_COLUMN_CENTER_INDEX + BOARD_COLUMN_NUMBER_BUFFER_SIZE)
	{
		cout << BOARD_COLUMN_NUMBER_BUFFER_SYMBOL;
	}
	// in any other case
	else
	{
		cout << BOARD_COLUMN_BOTTOM_SYMBOL;
	}
}

// function to display the vertical Field divisors
void displayVerticalFieldDivisors(int y)
{
	// if y not on the horizontal Field divisor
	if (y % BOARD_ROW_DIVIDER_INDEX != 0)
	{
		cout << BOARD_COLUMN_EDGE_SYMBOL;
	}
	else
	{
		cout << BOARD_ROW_DIVIDER_SYMBOL;
	}
}

// function to display the horizontal Field divisors
void displayHorizontalFieldDivisors(int y)
{
	// if y not on the top
	if (y != 0)
	{
		cout << BOARD_COLUMN_BOTTOM_SYMBOL;
	}
	else
	{
		cout << BOARD_COLUMN_TOP_SYMBOL;
	}
}

// function to display Fields
void displayFields(vector<Field*> allFields, int x, int y)
{
	// iterate through all the Field instances in 'allFields'
	for (int field = 0; field < allFields.size(); field++)
	{
		// extract the coordinates of a particular field
		vector<Coordinates> allCoordinates = allFields[field]->getAllCooridnates();

		// iterate through all the coordinates
		for (int cord = 0; cord < allCoordinates.size(); cord++)
		{
			// extract the coordinates for a particlar point
			Coordinates coordinates = allCoordinates[cord];

			// if the coordinates of a Field point match the coordinates on the board
			if (coordinates.x == x && coordinates.y == y)
			{
				// display the symbol of a Field
				cout << allFields[field]->getSymbol();
			}
		}
	}
}

// function to display the outline of the board and all the Fields
void displayOutline(vector<Field*> allFields)
{
	int iterator = 0;
	int columnNumberDisplay;

	// the whole board space (top -> bottom), with the bottom display
	for (int y = 0; y < BOARD_HEIGHT; y++)
	{
		// the whole board space (left -> right)
		for (int x = 0; x < BOARD_WIDTH; x++)
		{
			// determine column numbers
			columnNumberDisplay = x - BOARD_COLUMN_NUMBER_DISPLAY_SUBTRACTOR - (iterator * BOARD_COLUMN_NUMBER_DISPLAY_MULTIPLIER);

			// if y on the bottom display
			if (y == BOARD_HEIGHT_OPERATIONAL)
			{
				displayBottomDisplay(x, columnNumberDisplay, &iterator);
			}
			// if x on the vertical Field divisor
			else if (x % BOARD_COLUMN_WIDTH == 0)
			{
				displayVerticalFieldDivisors(y);
			}
			// if y on the horizontal Field divisor
			else if (y % BOARD_ROW_DIVIDER_INDEX == 0)
			{
				displayHorizontalFieldDivisors(y);
			}
			// if x on the right and left from vertical Field divisorss
			else if (x % BOARD_COLUMN_WIDTH == BOARD_COLUMN_EDGE_BUFFER_SIZE
				|| x % BOARD_COLUMN_WIDTH == BOARD_COLUMN_WIDTH - BOARD_COLUMN_EDGE_BUFFER_SIZE)
			{
				cout << BOARD_COLUMN_DIVISOR_BUFFER_SYMBOL;
			}
			// in any other case display Fields
			else
			{
				displayFields(allFields, x, y);
			}
		}

		cout << BOARD_ROW_SEPARATOR;
	}
}

// function to display the board (the outline as well as all the pawns)
void displayBoard(vector<Field*> allFields)
{
	cout << BOARD_HEADER;

	displayOutline(allFields);

	cout << BOARD_FOOTER;
}
