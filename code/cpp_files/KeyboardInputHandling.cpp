#include "../h_files/KeyboardInputHandling.h"
#include "../h_files/Definers.h"

#include <iostream>
#include <string>
#include <vector>

using namespace std;

// function to iterate over the vector of all possible inputs 'inputCharacters' to determine whether a given character stream
// 'characterStream' is included in this vector
bool checkKeyboardInputCorrectness(vector<char> inputCharacters, string characterStream)
{
	// check whether the vector of all characters is not empty and whether the size of character stream is equal 'OPTION_SIZE'
	if (!inputCharacters.empty() && characterStream.size() == CHECKING_INPUT_CORRECTNESS_OPTION_SIZE)
	{
		// iterate over the whole vector of possible inputs 
		for (int characterIndex = 0; characterIndex < inputCharacters.size(); characterIndex++)
		{
			// if a given character has a match in the vector of all possible inputs return 'CHECKING_INPUT_CORRECTNESS_CORRECT_INPUT'
			if (characterStream[CHECKING_INPUT_CORRECTNESS_OPTION_INDEX] == inputCharacters[characterIndex])
			{
				return CHECKING_INPUT_CORRECTNESS_CORRECT_INPUT;
			}
		}
	}

	// if after iterating over the whole vector no match has been found return 'CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT'
	return CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT;
}

//function to handle all the inputs from the keyboard (when there are options - single characters to choose)
char handleKeyboardInput(vector<char> inputCharacters)
{
	bool result;
	string characterStream;

	do
	{
		// read character stream from the console (input from the keyboard)
		cin >> characterStream;

		// check whether the input from the console matches that in the vector of possible inputs 'inputCharacters'
		result = checkKeyboardInputCorrectness(inputCharacters, characterStream);

		// if the input is incorrect display an appropriate message
		if (result == CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT)
		{
			cout << CHECKING_INPUT_CORRECTNESS_INCORRECT_INPUT_DISPLAY;
		}
	}
	// repeat the process until the input matches that in the vector of possible inputs 'inputCharacters'
	while (result != CHECKING_INPUT_CORRECTNESS_CORRECT_INPUT);

	// return the valid option from the character stream (a single character)
	return characterStream[CHECKING_INPUT_CORRECTNESS_OPTION_INDEX];
}
