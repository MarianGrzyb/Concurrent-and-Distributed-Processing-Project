// ============================================================
//  GameLogic.cpp — pure game logic, no UI, no sockets
// ============================================================

#include "../h_files/GameLogic.h"
#include "../h_files/Definers.h"

#include <iostream>
#include <vector>
using namespace std;

// ============================================================
//  pawnPlacing — verbatim from game.cpp
// ============================================================
void pawnPlacing(vector<Field*>& allFields, char symbol, int columnInput)
{
    int lowestRow = ROWS_NUMBER;
    int lowestRowIndex = 0;

    for (int fieldIndex = 0; fieldIndex < (int)allFields.size(); fieldIndex++)
    {
        Field field = *allFields[fieldIndex];

        if (field.getColumn() == columnInput && field.getRow() <= lowestRow && !field.getOccupied())
        {
            lowestRow = field.getRow();
            lowestRowIndex = fieldIndex;
        }
    }

    allFields[lowestRowIndex]->setOccupied(true);
    allFields[lowestRowIndex]->setSymbol(symbol);
}

// ============================================================
//  fieldAt — verbatim from game.cpp
// ============================================================
Field* fieldAt(const vector<Field*>& allFields, int row, int col)
{
    if (row < 1 || row > ROWS_NUMBER || col < 1 || col > COLUMNS_NUMBER)
        return nullptr;

    return allFields[(row - 1) * COLUMNS_NUMBER + (col - 1)];
}

// ============================================================
//  victory — verbatim from game.cpp
// ============================================================
bool victory(vector<Field*>& allFields, char symbol)
{
    int directions[4][2] = {
        {0,  1},
        {1,  0},
        {1,  1},
        {1, -1}
    };

    for (int i = 0; i < (int)allFields.size(); i++)
    {
        Field* origin = allFields[i];

        if (!origin->getOccupied() || origin->getSymbol() != symbol)
            continue;

        int originRow = origin->getRow();
        int originCol = origin->getColumn();

        for (int d = 0; d < 4; d++)
        {
            int count = 1;

            for (int step = 1; step < 4; step++)
            {
                Field* candidate = fieldAt(allFields,
                    originRow + step * directions[d][0],
                    originCol + step * directions[d][1]);

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

// ============================================================
//  isColumnAvailable
// ============================================================
bool isColumnAvailable(const vector<Field*>& allFields, int column)
{
    for (int i = 0; i < (int)allFields.size(); ++i)
    {
        Field* f = allFields[i];
        if (f->getRow() == ROWS_NUMBER &&
            f->getColumn() == column   &&
            !f->getOccupied())
        {
            return true;
        }
    }
    return false;
}

// ============================================================
//  getAvailableColumns
// ============================================================
vector<int> getAvailableColumns(const vector<Field*>& allFields)
{
    vector<int> available;
    for (int col = 1; col <= COLUMNS_NUMBER; ++col)
    {
        if (isColumnAvailable(allFields, col))
            available.push_back(col);
    }
    return available;
}

// ============================================================
//  determinePossibleMoves — verbatim from game.cpp
// ============================================================
int determinePossibleMoves(vector<Field*> allFields, vector<char>& validKeyboardInputs)
{
    int availableColumns = 0;
    validKeyboardInputs.push_back(TURN_QUIT_TO_MAIN_MENU_KEYCAP);

    for (int fieldIndex = 0; fieldIndex < (int)allFields.size(); fieldIndex++)
    {
        Field field = *allFields[fieldIndex];

        if (field.getRow() == ROWS_NUMBER && !field.getOccupied())
        {
            availableColumns++;
            cout << TURN_AVAILABLE_COLUMN_DISPLAY << field.getColumn() << TURN_AVAILABLE_COLUMN_FOOTER_DISPLAY;
            validKeyboardInputs.push_back(char(CASTING_TO_CHAR + field.getColumn()));
        }
    }

    if (availableColumns != 0)
    {
        cout << TURN_QUIT_TO_MAIN_MENU_DISPLAY;
    }

    return availableColumns;
}
