#pragma once

// ============================================================
//  GameClient.h
// ============================================================

#include <winsock2.h>
#include <windows.h>
#include <vector>
#include <string>
#include "Field.h"

using namespace std;

int            runGameClient();
SOCKET         connectToGameServer();
vector<Field*> rebuildFieldsFromSymbols(const vector<char>& symbols);
void           freeFields(vector<Field*>& allFields);
int            promptColumnChoice();
bool           handleSetupPhase(SOCKET sock, int& mySlot, string& myColour, string& myName);
