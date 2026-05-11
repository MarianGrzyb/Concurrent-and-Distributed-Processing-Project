#pragma once

//  constants needed for client - server communication

// variable sizes
#define PROTOCOL_PAYLOAD_SIZE 128
#define PROTOCOL_BOARD_CELLS 42  // 6 * 7
#define PROTOCOL_MESSAGE_SIZE (1 + PROTOCOL_PAYLOAD_SIZE)

// messages from server to client
#define MSG_ASSIGN_ID 'A'  // payload[0] = slot (1 or 2)
#define MSG_BOARD_STATE 'B'  // payload[0..41]=symbols, [42]=activeSlot
#define MSG_YOUR_TURN 'T'  // prompt: make a move
#define MSG_MOVE_INVALID 'I'  // server rejected the column
#define MSG_MOVE_OK 'K'  // server accepted the move
#define MSG_GAME_OVER 'G'  // payload[0] = result code

// constants for setups from server to client
#define MSG_CHOOSE_COLOUR 'L'  // ask client to pick a colour
#define MSG_COLOUR_TAKEN 'X'  // the colour client picked is taken, retry
#define MSG_ASK_NAME 'N'  // ask client to send their name
#define MSG_NAME_INVALID 'V'  // name too long, retry
#define MSG_SETUP_DONE 'S'  // setup complete; payload = player info string

// definers for client to server communication
#define MSG_COLUMN_CHOICE 'C'  // payload[0] = column char '1'..'7'
#define MSG_CLIENT_QUIT 'Q'  // client wants to abort
#define MSG_COLOUR_CHOICE 'O'  // payload[0] = 'b' or 'r'
#define MSG_NAME_CHOICE 'M'  // payload = name string (null-terminated)

// const codes for the game result
#define RESULT_WIN_PLAYER1 '1'
#define RESULT_WIN_PLAYER2 '2'
#define RESULT_DRAW 'D'

// protocol definers
#define PROTOCOL_PORT 55555
#define PROTOCOL_MAX_CLIENTS 2
#define PROTOCOL_LISTEN_BACKLOG 2

// definers for rematching, waiting in the lobby
#define MSG_PLAY_AGAIN_PROMPT 'P'  // server asks: play again?
#define MSG_PLAY_AGAIN_YES 'Y'  // client votes yes
#define MSG_PLAY_AGAIN_NO 'Z'  // client votes no  (server will close)

// definers to handle disconnection 
#define MSG_OPPONENT_QUIT 'F'  // opponent pressed Q — game over immediately
#define MSG_OPPONENT_DISCONNECTED 'D'  // opponent's terminal closed — wait for reconnect
#define MSG_RECONNECT_SUCCESS 'R'  // server tells remaining client reconnect succeeded
#define MSG_RECONNECT_FAILED 'E'  // 15s elapsed, no reconnect — game over
#define MSG_RECONNECT_HELLO 'H'  // reconnecting client identifies itself

// definers to handle reconnection
#define RECONNECT_TIMEOUT_SECONDS 30
#define RECONNECT_PORT 55556   // separate port for reconnect attempts

#define MSG_NORMAL_CONNECT '8'
#define MSG_RECONNECT_REQUIRED '9'
