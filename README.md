# Connect Four
## 1. A brief description of the game
This project is a terminal based implementation of the classic Connect Four game. Two players (Black and Red) take turns dropping their pawns into a grid. 
The game dynamically calculates the lowest available row in the chosen column and places the pawn. 
The goal of the game is to be the first player to form a horizontal, vertical, or diagonal line of four pawns. 
The game features a main menu, input validation, and colored console text for a better user experience.

## 2. Description of the project's file structure
The project is built using Object-Oriented Programming (OOP) principles, separating concerns into distinct C++ header (.h) and source (.cpp) files:

`game.cpp`: The main entry point of the program. It houses the primary game loop, turn management, pawn placement logic, and the algorithm to check for victory conditions across all four directions.

`Board.h` / `Board.cpp`: Handles the visual representation and rendering of the game grid in the console.

`Definers.h`: A central repository for macros, constants, and configuration values (e.g., board dimensions, string prompts).

`Field.h` / `Field.cpp`: Defines the Field class, representing a single cell on the board, tracking its coordinates and occupancy status.

`FieldsInitialization.h` / `FieldInitialization.cpp`: Handles the generation and initial setup of the game board's grid of Field objects.

`KeyboardInputHandling.h` / `KeyboardInputHandling.cpp`: Manages user input safely, ensuring only valid keystrokes are processed and preventing console crashes.

`MainMenu.h` / `MainMenu.cpp`: Controls the startup interface, allowing users to start a new game or exit.

`Player.h` / `Player.cpp`: Defines the Player class, holding player-specific data such as name, assigned color, and pawn symbol.

`PlayerInitialization.h` / `PlayerInitialization.cpp`: Manages the pre-game setup phase where player details are configured.

## 3. Listing the methods used in concurrent programming
##### Multithreading `(std::thread)`: 
Used to separate the UI/rendering loop from the network communication loop. While one thread handles updating the console screen and listening to local keyboard inputs, a separate background thread listens for incoming data packets (opponent moves) over the network.

##### Concurrency Protections / Mutexes `(std::mutex)`: 
Used to prevent race conditions. When the network thread receives an opponent's move, it must update the shared allFields vector. Mutex locks ensure that the rendering thread and the network thread do not try to read and write to the board state at the exact same millisecond, which would cause a crash.

##### Asynchronous Socket Listening: 
Non-blocking sockets or asynchronous network polling are used so the server/client can constantly check for connection drops (e.g., if a client closes their window abruptly) and trigger the safety fallbacks designed to handle those crashes gracefully.



## 4. List of external libraries/frameworks used
This project relies purely on standard libraries and OS-specific APIs. 
C++ Standard Template Library (STL):
- `<iostream>` : For standard console input and output.
- `<vector>` : For dynamic arrays, heavily used to manage the grid of Field objects and the list of Player objects.
- `<thread>` & `<mutex>`: For implementing the multithreading and concurrency controls required by the client-server architecture.

## 5. Screenshots of the game
# todo 

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

