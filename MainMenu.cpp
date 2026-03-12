#include "MainMenu.h"
#include "Definers.h"
#include "KeyboardInputHandling.h"

#include <iostream>
#include <vector>

using namespace std;

// function to print the Main Menu
void displayMainMenu()
{
	// display all the possible options separately
	cout << MAIN_MENU_PRESS_DISPLAY << MAIN_MENU_START_NEW_GAME_KEYCAP << MAIN_MENU_START_NEW_GAME_DISPLAY;
	cout << MAIN_MENU_PRESS_DISPLAY << MAIN_MENU_QUIT_THE_GAME_KEYCAP << MAIN_MENU_QUIT_DISPLAY;
	// display end of the Main Menu
	cout << MAIN_MENU_FOOTER_DISPLAY;
}

// function to handle the Main Menu (all the possible options) and choose the valid option
char mainMenu() 
{
	// display all the options from the Main Menu
	displayMainMenu();

	// add all the possible valid options to 'validKeyboardInputs'
	vector<char> validKeyboardInputs;
	validKeyboardInputs.push_back(MAIN_MENU_START_NEW_GAME_KEYCAP);
	validKeyboardInputs.push_back(QUIT_THE_GAME_KEYCAP);

	// handle the input from the console (check whether the input is valid comparing it to all the possible options in 'validKeyboardInputs')
	// and return the valid option
	return handleKeyboardInput(validKeyboardInputs);
}
