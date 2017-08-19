#ifndef COMMAND_H
#define COMMAND_H

#define CMD_PARSE_ERR -1
#define CMD_EXEC_ERR 1

typedef enum commandType {
	CMD_READ, CMD_WRITE, CMD_APPEND
} CommandType_te;

typedef struct command {
	CommandType_te type;
	const char* fileName;
} Command_ts;

/*Inputs:
	char* -> a command string to be parsed
	Command_ts [] -> array of Command_ts to hold the parsed commands
	size_t -> size of Command_ts array
  Function:
	parses the command string into separate commands and stores them in
	the Command_ts array
  Returns:
	Number of commands stored in Command_ts array if successfully parses the
	input command string; CMD_PARSE_ERR otherwise. */
int Command_parse(char*, Command_ts [], size_t);


/*Inputs:
	Command_ts structure
  Function:
	Executes the command contained in the input Command_ts structure
  Returns:
	0 if successful, CMD_EXEC_ERR otherwise. */
int Command_execute(Command_ts);

#endif
