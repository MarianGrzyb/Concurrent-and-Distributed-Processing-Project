# Connect Four
## 1. A brief description of the game
This project is a terminal based implementation of the classic Connect Four game. Two players (Black and Red) take turns dropping their pawns into a grid. 
The game dynamically calculates the lowest available row in the chosen column and places the pawn. 
The goal of the game is to be the first player to form a horizontal, vertical, or diagonal line of four pawns. 
The game features a main menu, input validation, and colored console text for a better user experience.

## 2. Description of the project's file structure
The project is built using Object-Oriented Programming (OOP) principles, separating concerns into distinct C++ header (.h) and source (.cpp) files:

`game.cpp`: The main entry point of the program. It houses the primary game loop, turn management, pawn placement logic, and the algorithm to check for victory conditions across all four directions.
Functions:
- `main()`: Launches the application and handles the main menu loop.
- `startNewGame()`: Preparatory function to initialize players and launch the game loop.
- `mainGameLoop()`: Controls the flow of the game, alternating turns and checking for win/draw states.
- `turn()`: Handles the specific sequence of a single player's turn (displaying moves, capturing input, placing pawns).
- `pawnPlacing()`: Calculates the lowest available row in a chosen column and drops the pawn.
- `victory()`: A 4-directional algorithm that checks horizontal, vertical, and diagonal lines for a win condition.
- `determinePossibleMoves()`: Scans the board to show the player which columns are valid for their turn.
- `determinePlayerOrder()`: Sets who goes first based on chosen colors.



`Board.h` / `Board.cpp`: Handles the visual representation and rendering of the game grid in the console.
Functions:
- `void displayBoard()`
- `void displayBottomDisplay()` // function to display the bottom display of the board
- `void displayVerticalFieldDivisors()` // function to display the vertical Field divisors
- `void displayHorizontalFieldDivisors()` // function to display the horizontal Field divisors
- `void displayFields()` // function to display Fields, iterates through all the Field instances in 'allFields', then extract the coordinates of a particular field, then iterate through all the coordinates, then extract the coordinates for a particlar point, if the coordinates of a Field point match the coordinates on the board, at last display the symbol of a Field
- `void displayOutline()` // function to display the outline of the board and all the Fields
- `void displayBoard()` // function to display the board (the outline as well as all the pawns)

`Definers.h`: A central repository for macros, constants, and configuration values (e.g., board dimensions, string prompts).
Contents:
Defines board dimensions, UI text strings, standard keycaps for input, and color codes.


`Field.h` / `Field.cpp`: Defines the Field class, representing a single cell on the board, tracking its coordinates and occupancy status.


`FieldsInitialization.h` / `FieldInitialization.cpp`: Handles the generation and initial setup of the game board's grid of Field objects.
Functions:
- `initFields()` // function to initialize all the Fields and input them into a vector of Fields 'allFields'


`KeyboardInputHandling.h` / `KeyboardInputHandling.cpp`: Manages user input safely, ensuring only valid keystrokes are processed and preventing console crashes.
Functions: 
- `handleKeyboardInput()` // function to handle all the inputs from the keyboard (when there are options - single characters to choose)
- `checkKeyboardInputCorrectness()` // function to iterate over the vector of all possible inputs 'inputCharacters' to determine whether a given character stream


`MainMenu.h` / `MainMenu.cpp`: Controls the startup interface, allowing users to start a new game or exit.
Functions:
- `mainMenu()` // function to handle the Main Menu (all the possible options) and choose the valid option
- `displayMainMenu()` // function to print the Main Menu




`Player.h` / `Player.cpp`: Defines the Player class, holding player-specific data such as name, assigned color, and pawn symbol.
Functions: 
- `setInitialFieldSymbol()` // function to set the initial Field symbol based on colour
- `setDefaultName()` //function to set default name based on the order (id) of players



`PlayerInitialization.h` / `PlayerInitialization.cpp`: Manages the pre-game setup phase where player details are configured. 
Functions: 
- `determineTakenColours()` // function to determine which colours have been already taken by other players
- `determineAvailableColours()` // function making it impossible to choose already taken colour
- `setNameForThePlayer()` // function to set the name of the player from the console
- `initPlayers()` // function to initialize players and their attributes

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

