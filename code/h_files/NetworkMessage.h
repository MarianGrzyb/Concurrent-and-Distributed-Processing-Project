#pragma once

// ============================================================
//  NetworkMessage.h — build / parse / send / recv helpers
// ============================================================

#include "Protocol.h"
#include "Field.h"

#include <vector>
#include <string>
#include <winsock2.h>

using namespace std;

struct RawMessage
{
    char type;
    char payload[PROTOCOL_PAYLOAD_SIZE];
};

// ---- Game flow ---------------------------------------------
RawMessage buildAssignId(int playerSlot);
RawMessage buildBoardState(const vector<Field*>& allFields, int activePlayerSlot);
RawMessage buildYourTurn();
RawMessage buildMoveInvalid();
RawMessage buildMoveOk();
RawMessage buildGameOver(char resultCode);
RawMessage buildColumnChoice(int column);
RawMessage buildClientQuit();

// ---- Setup phase -------------------------------------------
RawMessage buildChooseColour(bool blackAvailable, bool redAvailable);
RawMessage buildColourTaken();
RawMessage buildAskName();
RawMessage buildNameInvalid();
RawMessage buildSetupDone(int slot, const string& colour, const string& name);

RawMessage buildColourChoice(char keycap);   // 'b' or 'r'
RawMessage buildNameChoice(const string& name);

// ---- Parse -------------------------------------------------
int            parseAssignId(const RawMessage& msg);
vector<char>   parseBoardSymbols(const RawMessage& msg);
int            parseActiveTurn(const RawMessage& msg);
int            parseColumnChoice(const RawMessage& msg);
char           parseGameOverResult(const RawMessage& msg);
char           parseColourChoice(const RawMessage& msg);   // returns 'b' or 'r'
string         parseNameChoice(const RawMessage& msg);
// parseChooseColour: returns pair<blackAvailable, redAvailable>
pair<bool,bool> parseChooseColour(const RawMessage& msg);
// parseSetupDone: fills slot, colour, name
void           parseSetupDone(const RawMessage& msg, int& slot, string& colour, string& name);

// ---- Transport ---------------------------------------------
bool sendMessage(SOCKET sock, const RawMessage& msg);
bool recvMessage(SOCKET sock, RawMessage& msg);

// ---- Rematch -----------------------------------------------
RawMessage buildPlayAgainPrompt();
RawMessage buildPlayAgainYes();
RawMessage buildPlayAgainNo();
