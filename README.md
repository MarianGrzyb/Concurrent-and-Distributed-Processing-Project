# Connect Four
## 1. A brief description of the game
This project is a terminal based implementation of the classic Connect Four game. Two players (Black and Red) take turns dropping their pawns into a grid. 
The game dynamically calculates the lowest available row in the chosen column and places the pawn. 
The goal of the game is to be the first player to form a horizontal, vertical, or diagonal line of four pawns. 
The game features a main menu, input validation, and colored console text for a better user experience.

## 2. Description of the project's file structure
The project is built using Object-Oriented Programming (OOP) principles, separating concerns into distinct C++ header (.h) and source (.cpp) files:

### Source Files (`cpp_files/`)
#### Entry Points:
`server_main.cpp` / `server_main_game.cpp`: The starting point for the server process.

`client_main_game.cpp`: The starting point for the client process.

`game.cpp`: Contains core game flow logic and player turn management.

#### Network Core:

`GameServer.cpp` / `GameClient.cpp`: Implementation of server-side and client-side high-level logic.

`NetworkMessage.cpp`: Handling of packet serialization and communication protocol.

`socket.cpp` / `server.cpp`: Low-level WinSock initialization and socket wrappers.

#### Game Logic & UI:

`GameLogic.cpp`: Implementation of the 4-directional victory algorithm and move validation.

`Board.cpp` / `Field.cpp`: Visual rendering of the board and management of individual field states.

`FieldInitialization.cpp` / `PlayerInitialization.cpp`: Logic for setting up the grid and player profiles.

`KeyboardInputHandling.cpp`: Secure handling of terminal inputs.

`MainMenu.cpp`: Logic for the initial user interface.

`Player.cpp`: Player class implementation (symbols, colors, IDs).

### Header Files (`h_files/`)

#### Configuration:

`Definers.h`: Global macros, UI text strings, and color codes.

`Protocol.h`: Shared network constants (ports, message types, buffer sizes).

#### Class Definitions:

Corresponding .h files for all major components listed above (e.g., `Board.h`, `Player.h`, `GameServer.h`, `socket.h`).

## 3. Listing the methods used in concurrent programming
The project implements a hybrid concurrency model, combining independent system processes with internal server multithreading to ensure a synchronized and non-blocking networked experience.

##### Multiprocessing (System Architecture)
The application operates using a three-process model managed by the OS:

- GameServer.exe: Acts as the central "source of truth." It maintains the authoritative board state and coordinates communication between players.

- GameClient.exe (x2): Two independent client processes. Each process has its own memory space and stack, ensuring that a crash or error in one client does not directly affect the server or the other player.

##### Multithreading `(std::thread)`: 
The server heavily utilizes threads to optimize I/O operations and the pre-game setup:
- Parallel Negotiation (negotiatePlayers): The server spawns two threads (t0 and t1) to simultaneously handle nickname and color selection for both clients.
- Asynchronous Broadcasting: Functions like broadcastBoardState and broadcastGameOver create ephemeral threads to send data packets. This allows the server to push updates to both players at the exact same time without waiting for the first socket's transmission to complete.
- Thread Synchronization: All helper threads are synchronized using the .join() method, guaranteeing that the server does not advance the game logic until all network buffers have been successfully processed.

##### Synchronization & Race Condition Protection:
To protect shared resources and ensure logical consistency, the server employs primitives from the <mutex> and `<condition_variable>` libraries:

- SetupContext: A shared structure used during the initialization phase, protected by a `std::mutex`. This prevents "race conditions" where two players might successfully claim the same color simultaneously.

- Condition Variables (`std::condition_variable`): Used to enforce the game's business rules. Player 2 is put into a wait state (`cv.wait`) until Player 1 has committed to a color choice, ensuring a predictable and bug-free setup flow without wasting CPU cycles (busy-waiting).

- RAII Thread Safety: The code utilizes std::lock_guard and std::unique_lock to ensure that mutexes are automatically released, providing exception safety and preventing deadlocks



## 4. List of external libraries/frameworks used
The project is built using the C++ Standard Library and native Windows APIs to maintain high performance and a small footprint.

##### Networking & OS API
- WinSock2 (`winsock2.h`, `ws2tcpip.h`): The primary library for TCP/IP socket communication. It handles the low-level binary data exchange between the server and clients.
- Windows API (`windows.h`): Utilized for console manipulation, specifically SetConsoleTextAttribute for colored text UI, and for handling standard I/O streams.
- `ws2_32.lib`: The static library required for linking network functions in a Windows environment.

##### C++ Standard Template Library (STL)
- `<thread>`: For parallelizing network tasks and managing multiple client connections.
- `<mutex>` & `<condition_variable>`: For implementing thread-safe logic and event-driven synchronization.
- `<vector>` & `<string>`: Used for dynamic management of the game board grid and network message buffers.
- `<cstring>`: For low-level memory operations on network packet payloads (memset, strncpy).

## 5. Screenshots of the game
<img width="489" height="185" alt="image" src="https://github.com/user-attachments/assets/8ebf0a6a-addb-412b-a8cf-73b01b2c9a88" />
<img width="1700" height="1009" alt="image" src="https://github.com/user-attachments/assets/c45f9715-94b8-458d-b282-0f1a3d689de3" />
<img width="1698" height="1009" alt="image" src="https://github.com/user-attachments/assets/12884a7b-7521-42ff-b444-7bae9b4d9fb4" />
<img width="2370" height="365" alt="image" src="https://github.com/user-attachments/assets/5e888487-47cc-4089-8bc5-e7574e3eb9ca" />
<img width="645" height="294" alt="image" src="https://github.com/user-attachments/assets/f70ec814-73e7-4c09-b56e-9b1652a41a77" />


## 6. Description of the contributions of individual group members

### Mateusz Barszczewski
- Initial Implementation: Handled input handling and color display, created the Main Menu, and managed player initialization.
- Game Mechanics: Developed the Board (mapping fields and display) and the Main Game Loop, including turn handling and draw handling.
- Maintenance: Responsible for bug removal and overall code refinements.

### Zuzanna Warchoł
- Documentation: Authored the README file, including the game description, project file structure, concurrent programming methods, external libraries/frameworks list, screenshots, and group contributions.
- Client-Server Communication: Application of protections in case of connection breaks and handling unusual situations, bugs, or crashes.
- Testing: Conducted testing of the game.

### Tomasz Kruczalak
- Initial Implementation: Implemented player name changing, victory handling, and game quitting logic.
- Client-Server Communication: Ensured correct implementation of methods allowing client-server communication.
- Multithreading: applied appropriate multithreading and/or multiprocessing, and provided justification for the use of specific communication methods.

