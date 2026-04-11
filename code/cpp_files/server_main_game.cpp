// ============================================================
//  server_main_game.cpp
//
//  Entry point for the SERVER binary.
//  Replace (or add alongside) your existing server_main.cpp.
//  Link together with:
//    GameServer.cpp  NetworkMessage.cpp
//    Field.cpp  FieldsInitialization.cpp  Player.cpp
//    game.cpp  (for pawnPlacing() and victory())
//  Plus: ws2_32.lib
// ============================================================

#include "../h_files/GameServer.h"

int main()
{
    return runGameServer();
}
