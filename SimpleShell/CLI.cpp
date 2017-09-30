#include <string>
#include <sstream>
#include <iostream>

#include "CLI.h"

bool CLI::getInput(std::string array[], size_t arrSize, size_t* commands) {
	std::cout << prompt << ' ';

	std::string input;
	std::getline(std::cin, input);

	std::istringstream inputString(input);
	size_t i;
	for(i = 0;
		i < arrSize && std::getline(inputString, array[i], '|');
		i++);

	*commands = i;

	return input != "exit";
}
