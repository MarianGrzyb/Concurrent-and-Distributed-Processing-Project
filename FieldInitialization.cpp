#include "FieldsInitialization.h"
#include "Definers.h"
#include "Field.h"

#include <iostream>
#include <vector>

using namespace std;

// function to establish valid coordinates for Fields and to map them to appropriate rows and columns
vector<vector<vector<Coordinates>>>determineValidCoordinates()
{
	vector<vector<vector<Coordinates>>> coordinatesRowColumn(ROWS_NUMBER, vector<vector<Coordinates>>(COLUMNS_NUMBER));

	int row = ROWS_NUMBER;

	for (int y = 0; y < BOARD_HEIGHT_OPERATIONAL; y++)
	{
		int xCounter = 1;
		bool includeX = false;
		int column = 0;

		for (int x = 0; x < BOARD_WIDTH_OPERATIONAL; x++)
		{
			// if the coordinates are valid (should be included in the Field) put them in the 'coordinatesRowColumn'
			if (y % FIELD_Y_DIVISOR_INCREMENT != 0 && includeX)
			{
				Coordinates coordinates;
				coordinates.x = x;
				coordinates.y = y;
				// 'coordinatesRowColumn' holds the coordinates corresponding to row and column numbers, but starting from 0, not 1
				coordinatesRowColumn[row - FIELD_INDEX_INCREMENT][column - FIELD_INDEX_INCREMENT].push_back(coordinates);
			}

			xCounter += 1;

			// if the counter reaches the divisor reset counter 'xCounter' and flip the flag 'includeX'
			if (xCounter == FIELD_X_DIVISOR_INCREMENT)
			{
				xCounter = 0;

				if (includeX)
				{
					includeX = false;
				}
				else
				{
					includeX = true;
					// if the flag changes to 'true' the column number increases
					column += 1;
				}
			}
		}

		// if the divisor is reached row number decreases
		if (y % FIELD_Y_DIVISOR_INCREMENT == 0 && y != 0)
		{
			row -= 1;
		}
	}

	return coordinatesRowColumn;
}

// function to initialize all the Fields and input them into a vector of Fields 'allFields'
vector<Field*> initFields()
{
	vector<Field*> allFields;
	// 2-dimensional vector representing the board holding vectors with all the coordinates for a single Field
	vector<vector<vector<Coordinates>>> coordinatesRowColumn = determineValidCoordinates();

	// Fields initialization having all the coordinates
	for (int row = 0; row < ROWS_NUMBER; row++)
	{
		for (int column = 0; column < COLUMNS_NUMBER; column++)
		{
			// initialize all the Fields based on the 'coordinatesRowColumn' holding all the valid coordinates
			// row and column numbers are passed to the Fields not by index, but by valid number
			Field* field = new Field(row + FIELD_INDEX_INCREMENT, column + FIELD_INDEX_INCREMENT, coordinatesRowColumn[row][column]);
			allFields.push_back(field);
		}
	}

	return allFields;
}
