#include "KeyboardInputHandling.h"
#include "Definers.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// function to iterate over the vector of all possible inputs 'inputCharacters' to determine whether a given character stream
// 'characterStream' is included in this vector
bool checkKeyboardInputCorrectness(vector<char> inputCharacters, string characterStream)
{
	// check whether the vector of all characters is not empty and whether the size of character stream is equal 'OPTION_SIZE'
	if (!inputCharacters.empty() && characterStream.size() == OPTION_SIZE)
	{
		// iterate over the whole vector of possible inputs 
		for (int i = 0; i < inputCharacters.size(); i++)
		{
			// if a given character has a match in the vector of all possible inputs return 'CORRECT_INPUT'
			if (characterStream[0] == inputCharacters[i])
			{
				return CORRECT_INPUT;
			}
		}
	}

	// if after iteration over the whole vector no match has been found return 'INCORRECT_INPUT'
	return INCORRECT_INPUT;
}

//function to handle all the inputs from the keyboard (in any situation)
char handleKeyboardInput(vector<char> inputCharacters)
{
	bool result;
	string characterStream;

	do
	{
		// read character Stream from the console
		cin >> characterStream;

		// check whether the input from the console is correct
		result = checkKeyboardInputCorrectness(inputCharacters, characterStream);

		// if the input is incorrect display 'INCORRECT_INPUT_DISPLAY'
		if (result == INCORRECT_INPUT)
		{
			cout << INCORRECT_INPUT_DISPLAY;
		}
	}
	// repeat the process until the input is correct
	while (result != CORRECT_INPUT);

	return characterStream[0];
}