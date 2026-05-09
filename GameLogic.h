#pragma once

// ============================================================
//  GameLogic.h
//  Pure game-logic shared by GameServer (and optionally client).
//  No UI, no sockets, no main().
// ============================================================

#include "Field.h"
#include <vector>

using namespace std;

// Places a pawn in the lowest unoccupied row of the chosen column.
void pawnPlacing(vector<Field*>& allFields, char symbol, int columnInput);

// Returns the Field* at (row, col) or nullptr if out of bounds.
Field* fieldAt(const vector<Field*>& allFields, int row, int col);

// Returns true if four connected fields with 'symbol' exist.
bool victory(vector<Field*>& allFields, char symbol);

// Returns true if the top row of 'column' is unoccupied.
bool isColumnAvailable(const vector<Field*>& allFields, int column);

// Returns a vector of available column numbers (1..COLUMNS_NUMBER).
vector<int> getAvailableColumns(const vector<Field*>& allFields);

// Mirrors determinePossibleMoves() from game.cpp.
// Prints available columns, fills validKeyboardInputs (including quit key).
// Returns number of available columns.
int determinePossibleMoves(vector<Field*> allFields, vector<char>& validKeyboardInputs);
