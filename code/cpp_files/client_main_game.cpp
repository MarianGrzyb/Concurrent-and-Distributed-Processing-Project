// ============================================================
//  client_main_game.cpp
//
//  Entry point for the CLIENT binary.
//  Replace (or add alongside) your existing game main.cpp.
//  Link together with:
//    GameClient.cpp  NetworkMessage.cpp
//    Field.cpp  FieldsInitialization.cpp  Board.cpp
//  Plus: ws2_32.lib
//
//  Note: The client binary does NOT need game.cpp / Player.cpp /
//  PlayerInitialization.cpp — all game logic lives on the server.
// ============================================================

#include "../h_files/GameClient.h"

int main()
{
    return runGameClient();
}
