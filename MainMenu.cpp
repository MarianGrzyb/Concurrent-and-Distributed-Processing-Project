#include "MainMenu.h"
#include "Definers.h"
#include "KeyboardInputHandling.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// function to print the Main Menu
void printMainMenu()
{
	cout << MAIN_MENU_START_NEW_GAME_DISPLAY;
	cout << MAIN_MENU_QUIT_DISPLAY;
}

// function to handle the Main Menu (all the possible options)
string mainMenu() 
{
	// print all the options from the Main Menu
	printMainMenu();

	// add all the possible valid options to 'keyboardInputs'
	vector<char> validKeyboardInputs;
	validKeyboardInputs.push_back(MAIN_MENU_START_NEW_GAME_KEYCAP);
	validKeyboardInputs.push_back(QUIT_THE_GAME_KEYCAP);

	// handle the input from the console
	string validInput = HandleKeyboardInput(validKeyboardInputs);

	return validInput;
}