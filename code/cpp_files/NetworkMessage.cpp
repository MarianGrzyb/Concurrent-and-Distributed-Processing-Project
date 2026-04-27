// ============================================================
//  NetworkMessage.cpp
// ============================================================

#include "../h_files/NetworkMessage.h"
#include "../h_files/Definers.h"

#include <cstring>
#include <iostream>

using namespace std;

static RawMessage emptyMsg(char type)
{
    RawMessage m;
    m.type = type;
    memset(m.payload, 0, PROTOCOL_PAYLOAD_SIZE);
    return m;
}

// ============================================================
//  Build — game flow
// ============================================================

RawMessage buildAssignId(int playerSlot)
{
    RawMessage m = emptyMsg(MSG_ASSIGN_ID);
    m.payload[0] = static_cast<char>(playerSlot);
    return m;
}

RawMessage buildBoardState(const vector<Field*>& allFields, int activePlayerSlot)
{
    RawMessage m = emptyMsg(MSG_BOARD_STATE);
    for (int i = 0; i < (int)allFields.size() && i < PROTOCOL_BOARD_CELLS; ++i)
        m.payload[i] = allFields[i]->getSymbol();
    m.payload[PROTOCOL_BOARD_CELLS] = static_cast<char>('0' + activePlayerSlot);
    return m;
}

RawMessage buildYourTurn()      { return emptyMsg(MSG_YOUR_TURN);    }
RawMessage buildMoveInvalid()   { return emptyMsg(MSG_MOVE_INVALID);  }
RawMessage buildMoveOk()        { return emptyMsg(MSG_MOVE_OK);       }

RawMessage buildGameOver(char resultCode)
{
    RawMessage m = emptyMsg(MSG_GAME_OVER);
    m.payload[0] = resultCode;
    return m;
}

RawMessage buildColumnChoice(int column)
{
    RawMessage m = emptyMsg(MSG_COLUMN_CHOICE);
    m.payload[0] = static_cast<char>('0' + column);
    return m;
}

RawMessage buildClientQuit() { return emptyMsg(MSG_CLIENT_QUIT); }

// ============================================================
//  Build — setup phase
// ============================================================

// payload[0] = blackAvailable ('1'/'0'), payload[1] = redAvailable ('1'/'0')
RawMessage buildChooseColour(bool blackAvailable, bool redAvailable)
{
    RawMessage m = emptyMsg(MSG_CHOOSE_COLOUR);
    m.payload[0] = blackAvailable ? '1' : '0';
    m.payload[1] = redAvailable   ? '1' : '0';
    return m;
}

RawMessage buildColourTaken()  { return emptyMsg(MSG_COLOUR_TAKEN); }
RawMessage buildAskName()      { return emptyMsg(MSG_ASK_NAME);     }
RawMessage buildNameInvalid()  { return emptyMsg(MSG_NAME_INVALID);  }

// payload: "slot|colour|name"  e.g. "1|BLACK|Alice"
RawMessage buildSetupDone(int slot, const string& colour, const string& name)
{
    RawMessage m = emptyMsg(MSG_SETUP_DONE);
    string info = to_string(slot) + "|" + colour + "|" + name;
    // copy up to PROTOCOL_PAYLOAD_SIZE-1 chars
    strncpy(m.payload, info.c_str(), PROTOCOL_PAYLOAD_SIZE - 1);
    return m;
}

RawMessage buildColourChoice(char keycap)
{
    RawMessage m = emptyMsg(MSG_COLOUR_CHOICE);
    m.payload[0] = keycap;
    return m;
}

RawMessage buildNameChoice(const string& name)
{
    RawMessage m = emptyMsg(MSG_NAME_CHOICE);
    strncpy(m.payload, name.c_str(), PROTOCOL_PAYLOAD_SIZE - 1);
    return m;
}

// ============================================================
//  Parse
// ============================================================

int parseAssignId(const RawMessage& msg)
{
    return static_cast<int>(msg.payload[0]);
}

vector<char> parseBoardSymbols(const RawMessage& msg)
{
    vector<char> s;
    s.reserve(PROTOCOL_BOARD_CELLS);
    for (int i = 0; i < PROTOCOL_BOARD_CELLS; ++i)
        s.push_back(msg.payload[i]);
    return s;
}

int parseActiveTurn(const RawMessage& msg)
{
    return static_cast<int>(msg.payload[PROTOCOL_BOARD_CELLS] - '0');
}

int parseColumnChoice(const RawMessage& msg)
{
    return static_cast<int>(msg.payload[0] - '0');
}

char parseGameOverResult(const RawMessage& msg)
{
    return msg.payload[0];
}

char parseColourChoice(const RawMessage& msg)
{
    return msg.payload[0];   // 'b' or 'r'
}

string parseNameChoice(const RawMessage& msg)
{
    // payload is null-terminated string
    return string(msg.payload);
}

pair<bool,bool> parseChooseColour(const RawMessage& msg)
{
    bool black = (msg.payload[0] == '1');
    bool red   = (msg.payload[1] == '1');
    return {black, red};
}

void parseSetupDone(const RawMessage& msg, int& slot, string& colour, string& name)
{
    string info(msg.payload);
    // format: "slot|colour|name"
    size_t first = info.find('|');
    size_t second = info.find('|', first + 1);
    slot   = stoi(info.substr(0, first));
    colour = info.substr(first + 1, second - first - 1);
    name   = info.substr(second + 1);
}

// ============================================================
//  Transport
// ============================================================

bool sendMessage(SOCKET sock, const RawMessage& msg)
{
    const char* buf = reinterpret_cast<const char*>(&msg);
    int total = 0, toSend = PROTOCOL_MESSAGE_SIZE;
    while (total < toSend)
    {
        int sent = send(sock, buf + total, toSend - total, 0);
        if (sent == SOCKET_ERROR || sent == 0)
        {
            cerr << "[NET] sendMessage failed: " << WSAGetLastError() << endl;
            return false;
        }
        total += sent;
    }
    return true;
}

bool recvMessage(SOCKET sock, RawMessage& msg)
{
    char* buf = reinterpret_cast<char*>(&msg);
    int total = 0, toRecv = PROTOCOL_MESSAGE_SIZE;
    while (total < toRecv)
    {
        int got = recv(sock, buf + total, toRecv - total, 0);
        if (got == SOCKET_ERROR || got == 0)
        {
            if (got == 0) cerr << "[NET] recvMessage: connection closed." << endl;
            else          cerr << "[NET] recvMessage failed: " << WSAGetLastError() << endl;
            return false;
        }
        total += got;
    }
    return true;
}

// ---- Rematch -----------------------------------------------
RawMessage buildPlayAgainPrompt() { return emptyMsg(MSG_PLAY_AGAIN_PROMPT); }
RawMessage buildPlayAgainYes()    { return emptyMsg(MSG_PLAY_AGAIN_YES);    }
RawMessage buildPlayAgainNo()     { return emptyMsg(MSG_PLAY_AGAIN_NO);     }
