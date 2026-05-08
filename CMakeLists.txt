cmake_minimum_required(VERSION 3.10)
project(Concurrent_and_Distributed_Processing_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Server
        concurrent_distributed_game/server.cpp
        concurrent_distributed_game/server_main.cpp
        concurrent_distributed_game/server.h
        concurrent_distributed_game/Definers.h)

target_link_libraries(Server ws2_32)

add_executable(Client
        concurrent_distributed_game/Board.cpp
        concurrent_distributed_game/Board.h
        concurrent_distributed_game/Definers.h
        concurrent_distributed_game/Field.cpp
        concurrent_distributed_game/Field.h
        concurrent_distributed_game/FieldInitialization.cpp
        concurrent_distributed_game/FieldsInitialization.h
        concurrent_distributed_game/game.cpp
        concurrent_distributed_game/KeyboardInputHandling.cpp
        concurrent_distributed_game/KeyboardInputHandling.h
        concurrent_distributed_game/MainMenu.cpp
        concurrent_distributed_game/MainMenu.h
        concurrent_distributed_game/Player.cpp
        concurrent_distributed_game/Player.h
        concurrent_distributed_game/PlayerInitialization.cpp
        concurrent_distributed_game/PlayerInitialization.h
        concurrent_distributed_game/socket.cpp
        concurrent_distributed_game/socket.h
)

target_link_libraries(Client ws2_32)

add_executable(GameServer
        concurrent_distributed_game/server_main_game.cpp
        concurrent_distributed_game/GameServer.cpp
        concurrent_distributed_game/GameServer.h
        concurrent_distributed_game/NetworkMessage.cpp
        concurrent_distributed_game/NetworkMessage.h
        concurrent_distributed_game/Protocol.h
        concurrent_distributed_game/GameLogic.cpp
        concurrent_distributed_game/GameLogic.h
        concurrent_distributed_game/Field.cpp
        concurrent_distributed_game/Field.h
        concurrent_distributed_game/FieldInitialization.cpp
        concurrent_distributed_game/FieldsInitialization.h
        concurrent_distributed_game/Player.cpp
        concurrent_distributed_game/Player.h
        concurrent_distributed_game/Definers.h
)

target_link_libraries(GameServer ws2_32)

add_executable(GameClient
        concurrent_distributed_game/client_main_game.cpp
        concurrent_distributed_game/GameClient.cpp
        concurrent_distributed_game/GameClient.h
        concurrent_distributed_game/NetworkMessage.cpp
        concurrent_distributed_game/NetworkMessage.h
        concurrent_distributed_game/Protocol.h
        concurrent_distributed_game/GameLogic.cpp
        concurrent_distributed_game/GameLogic.h
        concurrent_distributed_game/Field.cpp
        concurrent_distributed_game/Field.h
        concurrent_distributed_game/FieldInitialization.cpp
        concurrent_distributed_game/FieldsInitialization.h
        concurrent_distributed_game/Board.cpp
        concurrent_distributed_game/Board.h
        concurrent_distributed_game/Definers.h
)

target_link_libraries(GameClient ws2_32)
