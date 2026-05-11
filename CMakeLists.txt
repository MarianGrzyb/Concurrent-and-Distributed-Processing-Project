cmake_minimum_required(VERSION 3.10)
project(Concurrent_and_Distributed_Processing_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Server
        server.cpp
        server_main.cpp
        server.h
        Definers.h)

target_link_libraries(Server ws2_32)

add_executable(Client
        Board.cpp
        Board.h
        Definers.h
        Field.cpp
        Field.h
        FieldInitialization.cpp
        FieldsInitialization.h
        game.cpp
        KeyboardInputHandling.cpp
        KeyboardInputHandling.h
        MainMenu.cpp
        MainMenu.h
        Player.cpp
        Player.h
        PlayerInitialization.cpp
        PlayerInitialization.h
        socket.cpp
        socket.h
)

target_link_libraries(Client ws2_32)

add_executable(GameServer
        server_main_game.cpp
        GameServer.cpp
        GameServer.h
        NetworkMessage.cpp
        NetworkMessage.h
        Protocol.h
        GameLogic.cpp
        GameLogic.h
        Field.cpp
        Field.h
        FieldInitialization.cpp
        FieldsInitialization.h
        Player.cpp
        Player.h
        Definers.h
)

target_link_libraries(GameServer ws2_32)

add_executable(GameClient
        client_main_game.cpp
        GameClient.cpp
        GameClient.h
        NetworkMessage.cpp
        NetworkMessage.h
        Protocol.h
        GameLogic.cpp
        GameLogic.h
        Field.cpp
        Field.h
        FieldInitialization.cpp
        FieldsInitialization.h
        Board.cpp
        Board.h
        Definers.h
)

target_link_libraries(GameClient ws2_32)
