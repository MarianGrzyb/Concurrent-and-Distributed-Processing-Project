#include "FieldsInitialization.h"
#include "Definers.h"
#include "Field.h"

#include <iostream>
#include <vector>

using namespace std;

vector<vector<vector<Coordinates>>>determineValidCoordinates()
{
	vector<vector<vector<Coordinates>>> coordinatesRowColumn(ROWS_NUMBER, vector<vector<Coordinates>>(COLUMNS_NUMBER));

	int row = ROWS_NUMBER;

	for (int y = 0; y < BOARD_HEIGHT_OPERATIONAL; y++)
	{
		int x_counter = 1;
		bool include_x = false;
		int column = 0;

		for (int x = 0; x < BOARD_WIDTH_OPERATIONAL; x++)
		{
			// if the coordinates are valid (should be included in the Field) put them in the 'coordinatesRowColumn'
			if (y % Y_DIVISOR_INCREMENT != 0 && include_x)
			{
				Coordinates coordinates;
				coordinates.x = x;
				coordinates.y = y;
				// 'coordinatesRowColumn' holds the coordinates corresponding to row and column numbers, but starting from 0, not 1
				coordinatesRowColumn[row - 1][column - 1].push_back(coordinates);
			}

			x_counter += 1;

			// if the counter reaches the divisor reset counter 'x_counter' and flip the flag 'include_x'
			if (x_counter == X_DIVISOR_INCREMENT)
			{
				x_counter = 0;

				if (include_x)
				{
					include_x = false;
				}
				else
				{
					include_x = true;
					// if the flag changes to 'true' the column number increases
					column += 1;
				}
			}
		}

		// if the divisor is reached row number decreases
		if (y % Y_DIVISOR_INCREMENT == 0 && y != 0)
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

	// fields initialization having all the coordinates
	for (int row = 0; row < ROWS_NUMBER; row++)
	{
		for (int column = 0; column < COLUMNS_NUMBER; column++)
		{
			// initialize all the fields based on the 'coordinatesRowColumn' holding all the valid coordinates
			// row and column numbers are passed to the Fields not by index, but by valid number
			Field* field = new Field(row + 1, column + 1, coordinatesRowColumn[row][column]);
			allFields.push_back(field);
		}
	}

	return allFields;
}