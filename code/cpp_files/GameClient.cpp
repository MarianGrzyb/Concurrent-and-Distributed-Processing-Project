// ============================================================
//  GameClient.cpp
//  Mirrors game.cpp main() do-while loop with mainMenu().
// ============================================================

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <vector>
#include <string>

#include "../h_files/GameClient.h"
#include "../h_files/NetworkMessage.h"
#include "../h_files/Protocol.h"
#include "../h_files/Definers.h"
#include "../h_files/Field.h"
#include "../h_files/Board.h"
#include "../h_files/FieldsInitialization.h"
#include "../h_files/GameLogic.h"

using namespace std;

// ============================================================
//  connectToGameServer
// ============================================================
SOCKET connectToGameServer()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        cerr << "[CLIENT] WSAStartup failed." << endl;
        return INVALID_SOCKET;
    }

    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET)
    {
        cerr << "[CLIENT] socket() failed: " << WSAGetLastError() << endl;
        WSACleanup();
        return INVALID_SOCKET;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port   = htons(PROTOCOL_PORT);
    InetPton(AF_INET, "127.0.0.1", &serverAddr.sin_addr.s_addr);

    cout << "[CLIENT] Connecting to server on port " << PROTOCOL_PORT << "..." << endl;

    if (connect(sock, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR)
    {
        cerr << "[CLIENT] connect() failed: " << WSAGetLastError() << endl;
        closesocket(sock);
        WSACleanup();
        return INVALID_SOCKET;
    }

    cout << "[CLIENT] Connected!" << endl;
    return sock;
}

// ============================================================
//  rebuildFieldsFromSymbols
// ============================================================
vector<Field*> rebuildFieldsFromSymbols(const vector<char>& symbols)
{
    vector<Field*> allFields = initFields();
    for (int i = 0; i < (int)allFields.size() && i < PROTOCOL_BOARD_CELLS; ++i)
    {
        char sym = symbols[i];
        allFields[i]->setSymbol(sym);
        allFields[i]->setOccupied(sym != FIELD_UNOCCUPIED_SYMBOL);
    }
    return allFields;
}

void freeFields(vector<Field*>& allFields)
{
    for (auto f : allFields) delete f;
    allFields.clear();
}

// ============================================================
//  promptColumnChoice
// ============================================================
int promptColumnChoice()
{
    while (true)
    {
        cout << "\nEnter column [1-" << COLUMNS_NUMBER << "]: ";
        string input;
        cin >> input;
        if (input.size() == 1 && input[0] >= '1' && input[0] <= ('0' + COLUMNS_NUMBER))
            return input[0] - '0';
        cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
    }
}

// ============================================================
//  handleSetupPhase — colour + name negotiation
//  Mirrors PlayerInitialization.cpp UI exactly.
// ============================================================
bool handleSetupPhase(SOCKET sock, int& mySlot, string& myColour, string& myName)
{
    while (true)
    {
        RawMessage msg;
        if (!recvMessage(sock, msg))
            return false;

        if (msg.type == MSG_CHOOSE_COLOUR)
        {
            auto [blackAvailable, redAvailable] = parseChooseColour(msg);

            cout << PLAYER_CHOOSE_COLOUR_DISPLAY;

            if (blackAvailable)
                cout << PLAYER_CHOOSE_COLOUR_BLACK_AVAILABLE_DISPLAY;
            else
                cout << PLAYER_CHOOSE_COLOUR_BLACK_UNAVAILABLE_DISPLAY;

            if (redAvailable)
                cout << PLAYER_CHOOSE_COLOUR_RED_AVAILABLE_DISPLAY;
            else
                cout << PLAYER_CHOOSE_COLOUR_RED_UNAVAILABLE_DISPLAY;

            cout << PLAYER_CHOOSE_COLOUR_FOOTER_DISPLAY;

            vector<char> validInputs;
            if (blackAvailable) validInputs.push_back(COLOUR_BLACK_KEYCAP);
            if (redAvailable)   validInputs.push_back(COLOUR_RED_KEYCAP);

            char chosen = 0;
            while (!chosen)
            {
                string input;
                cin >> input;
                if (input.size() == 1)
                    for (char v : validInputs)
                        if (input[0] == v) { chosen = input[0]; break; }
                if (!chosen)
                    cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
            }

            sendMessage(sock, buildColourChoice(chosen));
            continue;
        }

        if (msg.type == MSG_COLOUR_TAKEN)
        {
            cout << "\nColour already taken, please choose again.\n";
            continue;
        }

        if (msg.type == MSG_ASK_NAME)
        {
            cout << PLAYER_NAME_CHANGE_CHANGE_THE_NAME_DISPLAY;
            cout << PLAYER_NAME_CHANGE_DO_NOT_CHANGE_THE_NAME_DISPLAY;
            cout << PLAYER_NAME_CHANGE_FOOTER_DISPLAY;

            char choice = 0;
            while (!choice)
            {
                string input;
                cin >> input;
                if (input.size() == 1 &&
                    (input[0] == PLAYER_NAME_CHANGE_CHANGE_THE_NAME_KEYCAP ||
                     input[0] == PLAYER_NAME_CHANGE_DO_NOT_CHANGE_THE_NAME_KEYCAP))
                    choice = input[0];
                else
                    cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
            }

            if (choice == PLAYER_NAME_CHANGE_DO_NOT_CHANGE_THE_NAME_KEYCAP)
            {
                sendMessage(sock, buildNameChoice(""));
            }
            else
            {
                string name;
                do {
                    cout << PLAYER_NAME_CHANGE_SET_NAME_DISPLAY;
                    cin >> name;
                    if ((int)name.size() > PLAYER_NAME_CHANGE_NAME_MAX_LENGTH)
                        cout << PLAYER_NAME_CHANGE_INVALID_NAME_LENGTH_DISPLAY;
                } while ((int)name.size() > PLAYER_NAME_CHANGE_NAME_MAX_LENGTH);

                sendMessage(sock, buildNameChoice(name));
            }
            continue;
        }

        if (msg.type == MSG_NAME_INVALID)
        {
            cout << PLAYER_NAME_CHANGE_INVALID_NAME_LENGTH_DISPLAY;
            continue;
        }

        if (msg.type == MSG_SETUP_DONE)
        {
            parseSetupDone(msg, mySlot, myColour, myName);

            cout << PLAYER_NAME_CHANGE_NEW_NAME_DISPLAY
                 << myName
                 << PLAYER_NAME_CHANGE_SET_NAME_FOOTER_DISPLAY;

            cout << "\nYou are Player " << mySlot << ' ';
            if (myColour == COLOUR_BLACK_NAME)
                cout << PLAYER_CHOOSE_COLOUR_BLACK_AVAILABLE_DISPLAY;
            else
                cout << PLAYER_CHOOSE_COLOUR_RED_AVAILABLE_DISPLAY;
            cout << "\nWaiting for game to start...\n";
            return true;
        }
    }
}

// ============================================================
//  showMainMenu
//  Mirrors mainMenu() from MainMenu.cpp.
//  Returns MAIN_MENU_START_NEW_GAME_KEYCAP or MAIN_MENU_QUIT_THE_GAME_KEYCAP.
// ============================================================
static char showMainMenu()
{
    cout << MAIN_MENU_START_NEW_GAME_DISPLAY;
    cout << MAIN_MENU_QUIT_DISPLAY;
    cout << MAIN_MENU_FOOTER_DISPLAY;

    while (true)
    {
        string input;
        cin >> input;
        if (input.size() == 1)
        {
            char c = input[0];
            if (c == MAIN_MENU_START_NEW_GAME_KEYCAP || c == MAIN_MENU_QUIT_THE_GAME_KEYCAP)
                return c;
        }
        cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
    }
}

// ============================================================
//  playOneGame
//  Mirrors mainGameLoop() on the client side:
//  receives board states, sends column choices.
//  Returns true when the game ends (win/draw/quit).
// ============================================================
static void playOneGame(SOCKET sock,
                        HANDLE consoleColour,
                        int mySlot,
                        const string& myColour,
                        const string& myName)
{
    bool running = true;

    // Cache the last board snapshot so MSG_YOUR_TURN can call determinePossibleMoves
    vector<char> lastSymbols(PROTOCOL_BOARD_CELLS, FIELD_UNOCCUPIED_SYMBOL);

    while (running)
    {
        RawMessage msg;
        if (!recvMessage(sock, msg))
        {
            cerr << "[CLIENT] Lost connection to server." << endl;
            running = false;
            break;
        }

        switch (msg.type)
        {
            // ---- Board update (mirrors displayBoard call in mainGameLoop) ----
            case MSG_BOARD_STATE:
            {
                lastSymbols          = parseBoardSymbols(msg);
                int activeSlot       = parseActiveTurn(msg);

                vector<Field*> allFields = rebuildFieldsFromSymbols(lastSymbols);
                displayBoard(allFields);
                freeFields(allFields);

                // Mirror turn() header display
                if (activeSlot == mySlot)
                    cout << TURN_COLOUR_DISPLAY << myColour
                         << TURN_PLAYER_DISPLAY << myName
                         << TURN_FOOTER_DISPLAY;
                else
                    cout << "  Waiting for opponent...\n";

                break;
            }

            // ---- Our turn (mirrors turn() from game.cpp exactly) ----
            case MSG_YOUR_TURN:
            {
                // Rebuild fields from cached snapshot so determinePossibleMoves works
                vector<Field*> allFields = rebuildFieldsFromSymbols(lastSymbols);

                // mirrors turn(): print possible moves with column numbers
                cout << TURN_POSSIBLE_MOVES_DISPLAY;
                vector<char> validKeyboardInputs;
                int availableColumns = determinePossibleMoves(allFields, validKeyboardInputs);
                freeFields(allFields);

                if (availableColumns == 0)
                {
                    // draw will come as MSG_GAME_OVER from server, just wait
                    break;
                }

                // Read input — mirrors handleKeyboardInput() + gameQuitting()
                char validInput = 0;
                while (!validInput)
                {
                    string input;
                    cin >> input;
                    if (input.size() == 1)
                    {
                        char c = input[0];
                        for (char v : validKeyboardInputs)
                            if (c == v) { validInput = c; break; }
                    }
                    if (!validInput)
                        cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
                }

                // Quit option — mirrors gameQuitting()
                if (validInput == TURN_QUIT_TO_MAIN_MENU_KEYCAP)
                {
                    cout << TURN_QUIT_TO_MAIN_MENU_PLAYER_DISPLAY
                         << myName
                         << TURN_QUIT_TO_MAIN_MENU_DESCRIPTION_DISPLAY;
                    sendMessage(sock, buildClientQuit());
                    running = false;
                    break;
                }

                // Send column choice
                sendMessage(sock, buildColumnChoice(int(validInput - CASTING_TO_CHAR)));
                break;
            }

            case MSG_MOVE_INVALID:
            {
                cout << "\n[!] Column is FULL or invalid.\n";

                // Show valid columns again using cached board state
                vector<Field*> allFields = rebuildFieldsFromSymbols(lastSymbols);
                cout << TURN_POSSIBLE_MOVES_DISPLAY;
                vector<char> validKeyboardInputs;
                determinePossibleMoves(allFields, validKeyboardInputs);
                freeFields(allFields);

                char validInput = 0;
                while (!validInput)
                {
                    string input;
                    cin >> input;
                    if (input.size() == 1)
                    {
                        char c = input[0];
                        for (char v : validKeyboardInputs)
                            if (c == v) { validInput = c; break; }
                    }
                    if (!validInput)
                        cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
                }

                if (validInput == TURN_QUIT_TO_MAIN_MENU_KEYCAP)
                {
                    cout << TURN_QUIT_TO_MAIN_MENU_PLAYER_DISPLAY
                         << myName
                         << TURN_QUIT_TO_MAIN_MENU_DESCRIPTION_DISPLAY;
                    sendMessage(sock, buildClientQuit());
                    running = false;
                }
                else
                {
                    sendMessage(sock, buildColumnChoice(int(validInput - CASTING_TO_CHAR)));
                }
                break;
            }

            case MSG_MOVE_OK:
                // Accepted — next message will be MSG_BOARD_STATE
                break;

            // ---- Game over (mirrors victory/draw display in mainGameLoop) ----
            case MSG_GAME_OVER:
            {
                char result = parseGameOverResult(msg);

                if (result == RESULT_DRAW)
                {
                    cout << DRAW_EVENT_DESCRIPTION_DISPLAY;
                }
                else
                {
                    int    winnerSlot   = (result == RESULT_WIN_PLAYER1) ? 1 : 2;

                    if (winnerSlot == mySlot)
                    {
                        SetConsoleTextAttribute(consoleColour, TEXT_COLOUR_GREEN);
                        cout << WIN_EVENT_PLAYER_DISPLAY << myName << WIN_EVENT_COLOUR_DISPLAY << myColour
                             << WIN_EVENT_FOOTER_DISPLAY;
                        SetConsoleTextAttribute(consoleColour, TEXT_COLOUR_DEFAULT);
                    }
                    else
                    {
                        SetConsoleTextAttribute(consoleColour, TEXT_COLOUR_RED);
                        cout << WIN_EVENT_PLAYER_DISPLAY << myName << WIN_EVENT_COLOUR_DISPLAY << myColour
                        << LOST_EVENT_FOOTER_DISPLAY;
                        SetConsoleTextAttribute(consoleColour, TEXT_COLOUR_DEFAULT);
                    }
                }

                running = false;
                break;
            }

            default:
                cerr << "[CLIENT] Unknown message type '" << msg.type << "' — ignoring." << endl;
                break;
        }
    }
}

// ============================================================
//  runGameClient
//  Outer loop mirrors game.cpp main():
//    do { mainMenu -> startNewGame } while(true)
// ============================================================
int runGameClient()
{
    SOCKET sock = connectToGameServer();
    if (sock == INVALID_SOCKET)
        return 1;

    HANDLE consoleColour = GetStdHandle(STD_OUTPUT_HANDLE);

    // Mirror game.cpp main() do-while
    do
    {
        // Show main menu — mirrors mainMenu()
        char currentOption = showMainMenu();

        if (currentOption == MAIN_MENU_QUIT_THE_GAME_KEYCAP)
        {
            // Tell server we're done
            sendMessage(sock, buildClientQuit());
            break;
        }

        // MAIN_MENU_START_NEW_GAME_KEYCAP — mirrors startNewGame()

        // Setup phase: colour + name (mirrors initPlayers())
        int    mySlot   = -1;
        string myColour = "";
        string myName   = "";

        if (!handleSetupPhase(sock, mySlot, myColour, myName))
        {
            cerr << "[CLIENT] Setup failed." << endl;
            break;
        }

        // Play the game (mirrors mainGameLoop())
        playOneGame(sock, consoleColour, mySlot, myColour, myName);

        // Server will send MSG_PLAY_AGAIN_PROMPT — mirrors the do-while check
        RawMessage prompt;
        if (!recvMessage(sock, prompt) || prompt.type != MSG_PLAY_AGAIN_PROMPT)
            break;

        // Ask locally whether to play again — mirrors the do-while in main()
        cout << "\n";
        cout << MAIN_MENU_START_NEW_GAME_DISPLAY;   // [s] to start new game
        cout << MAIN_MENU_QUIT_DISPLAY;              // [q] to quit
        cout << MAIN_MENU_FOOTER_DISPLAY;

        char again = 0;
        while (!again)
        {
            string input;
            cin >> input;
            if (input.size() == 1)
            {
                char c = input[0];
                if (c == MAIN_MENU_START_NEW_GAME_KEYCAP || c == MAIN_MENU_QUIT_THE_GAME_KEYCAP)
                    again = c;
            }
            if (!again)
                cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
        }

        if (again == MAIN_MENU_START_NEW_GAME_KEYCAP)
            sendMessage(sock, buildPlayAgainYes());
        else
        {
            sendMessage(sock, buildPlayAgainNo());
            break;
        }
    }
    while (true);

    closesocket(sock);
    WSACleanup();
    return 0;
}
