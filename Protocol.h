#pragma once

// ============================================================
//  Protocol.h  —  shared constants for client <-> server wire
// ============================================================

// ---------- sizes -------------------------------------------
#define PROTOCOL_PAYLOAD_SIZE      128
#define PROTOCOL_BOARD_CELLS       42      // ROWS_NUMBER(6) * COLUMNS_NUMBER(7)
#define PROTOCOL_MESSAGE_SIZE      (1 + PROTOCOL_PAYLOAD_SIZE)

// ---------- Server -> Client --------------------------------
#define MSG_ASSIGN_ID              'A'  // payload[0] = slot (1 or 2)
#define MSG_BOARD_STATE            'B'  // payload[0..41]=symbols, [42]=activeSlot
#define MSG_YOUR_TURN              'T'  // prompt: make a move
#define MSG_MOVE_INVALID           'I'  // server rejected the column
#define MSG_MOVE_OK                'K'  // server accepted the move
#define MSG_GAME_OVER              'G'  // payload[0] = result code

// Setup phase — Server -> Client
#define MSG_CHOOSE_COLOUR          'L'  // ask client to pick a colour
#define MSG_COLOUR_TAKEN           'X'  // the colour client picked is taken, retry
#define MSG_ASK_NAME               'N'  // ask client to send their name
#define MSG_NAME_INVALID           'V'  // name too long, retry
#define MSG_SETUP_DONE             'S'  // setup complete; payload = player info string

// ---------- Client -> Server --------------------------------
#define MSG_COLUMN_CHOICE          'C'  // payload[0] = column char '1'..'7'
#define MSG_CLIENT_QUIT            'Q'  // client wants to abort
#define MSG_COLOUR_CHOICE          'O'  // payload[0] = 'b' or 'r'
#define MSG_NAME_CHOICE            'M'  // payload = name string (null-terminated)

// ---------- game-over result codes --------------------------
#define RESULT_WIN_PLAYER1         '1'
#define RESULT_WIN_PLAYER2         '2'
#define RESULT_DRAW                'D'

// ---------- misc --------------------------------------------
#define PROTOCOL_PORT              55555
#define PROTOCOL_MAX_CLIENTS       2
#define PROTOCOL_LISTEN_BACKLOG    2

// ---------- Rematch / lobby ---------------------------------
#define MSG_PLAY_AGAIN_PROMPT      'P'  // server asks: play again?
#define MSG_PLAY_AGAIN_YES         'Y'  // client votes yes
#define MSG_PLAY_AGAIN_NO          'Z'  // client votes no  (server will close)
