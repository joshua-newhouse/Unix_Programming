#include <string>
#include <iostream>

#include "CLI.h"
#include "Command.h"

static const size_t MAX_CMDS = 3;

int main() {
	CLI cli('>');
	std::string commands[MAX_CMDS] = {"", "", ""};

	size_t nCommandsInLine = 0;
	while(cli.getInput(commands, MAX_CMDS, &nCommandsInLine))
		Command::process(commands, nCommandsInLine);

	return 0;
}
