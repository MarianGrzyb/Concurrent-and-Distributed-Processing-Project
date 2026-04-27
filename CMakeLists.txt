cmake_minimum_required(VERSION 3.10)
project(Concurrent_and_Distributed_Processing_Project)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Server
        code/cpp_files/server.cpp
        code/cpp_files/server_main.cpp
        code/h_files/server.h
        code/h_files/Definers.h)

target_link_libraries(Server ws2_32)

add_executable(Client
        code/cpp_files/Board.cpp
        code/h_files/Board.h
        code/h_files/Definers.h
        code/cpp_files/Field.cpp
        code/h_files/Field.h
        code/cpp_files/FieldInitialization.cpp
        code/h_files/FieldsInitialization.h
        code/cpp_files/game.cpp
        code/cpp_files/KeyboardInputHandling.cpp
        code/h_files/KeyboardInputHandling.h
        code/cpp_files/MainMenu.cpp
        code/h_files/MainMenu.h
        code/cpp_files/Player.cpp
        code/h_files/Player.h
        code/cpp_files/PlayerInitialization.cpp
        code/h_files/PlayerInitialization.h
        code/cpp_files/socket.cpp
        code/h_files/socket.h
)

target_link_libraries(Client ws2_32)

add_executable(GameServer
        code/cpp_files/server_main_game.cpp
        code/cpp_files/GameServer.cpp
        code/h_files/GameServer.h
        code/cpp_files/NetworkMessage.cpp
        code/h_files/NetworkMessage.h
        code/h_files/Protocol.h
        code/cpp_files/GameLogic.cpp
        code/h_files/GameLogic.h
        code/cpp_files/Field.cpp
        code/h_files/Field.h
        code/cpp_files/FieldInitialization.cpp
        code/h_files/FieldsInitialization.h
        code/cpp_files/Player.cpp
        code/h_files/Player.h
        code/h_files/Definers.h
)

target_link_libraries(GameServer ws2_32)

add_executable(GameClient
        code/cpp_files/client_main_game.cpp
        code/cpp_files/GameClient.cpp
        code/h_files/GameClient.h
        code/cpp_files/NetworkMessage.cpp
        code/h_files/NetworkMessage.h
        code/h_files/Protocol.h
        code/cpp_files/GameLogic.cpp
        code/h_files/GameLogic.h
        code/cpp_files/Field.cpp
        code/h_files/Field.h
        code/cpp_files/FieldInitialization.cpp
        code/h_files/FieldsInitialization.h
        code/cpp_files/Board.cpp
        code/h_files/Board.h
        code/h_files/Definers.h
)

target_link_libraries(GameClient ws2_32)
