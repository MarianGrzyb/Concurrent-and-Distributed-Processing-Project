#pragma once

// ============================================================
//  GameServer.h
// ============================================================

#include <winsock2.h>
#include <vector>
#include <string>
#include "Field.h"
#include "Player.h"
#include "GameLogic.h"

using namespace std;

int  runGameServer();
bool acceptClients(SOCKET listenSock, SOCKET clientSockets[2]);
bool negotiatePlayers(SOCKET clientSockets[2], vector<Player*>& players);
void broadcastBoardState(SOCKET clientSockets[2], const vector<Field*>& allFields, int activeSlot);
int  getValidatedMove(SOCKET clientSocket, SOCKET otherClientSocket, const vector<Field*>& allFields);
void broadcastGameOver(SOCKET clientSockets[2], char resultCode);
