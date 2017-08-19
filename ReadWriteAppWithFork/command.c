#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "command.h"

#define MAX_PTRS 15
int Command_parse(char* buf, Command_ts cmds[], size_t nCmds) {
	int ret_val = 0;

	size_t bLen = strlen(buf);

	/* Create char array to point at each lexeme of command in buffer
	  ptr_idx is used to index the array of lexemes */
	char* ptrArr[MAX_PTRS] = {NULL};
	size_t ptr_idx = 0;

	/*Init pointers to begin and end of command string
	  Assign first pointer in array to point at first lexeme in cmd */
	char* s = buf;
	char* end = &buf[bLen - 1];
	ptrArr[ptr_idx++] = s;

	/*Move through command string marking the end of each lexeme with 
	  \0 and assigning a pointer in array to point at beginning of each
	  lexeme */
	while(s < end) {
	if(*s == ' ') {
		*s++ = '\0';
		ptrArr[ptr_idx++] = s;
		}
		s++;
	}

	/*Parse the commands into the array of command structures*/
	size_t i, j;
	for(i = 0, j = 0; ptrArr[i] && i < MAX_PTRS && j < nCmds; i++) {
		if(*ptrArr[i] == '|') {
			j++;
		}
		else if(strcmp(ptrArr[i], "read") == 0) {
			i++;
			if(*ptrArr[i] == '<') {
				i++;
				cmds[j].type = CMD_READ;
				cmds[j].fileName = ptrArr[i];
			}
			else {
				fprintf(stderr,
					"Unrecognized command:  read %s\n", ptrArr[i]);
				ret_val = CMD_PARSE_ERR;
				goto CLEAN_UP;
			}
		}
		else if(strcmp(ptrArr[i], "write") == 0) {
			i++;
			if(*ptrArr[i] == '>') {
				i++;
				cmds[j].type = CMD_WRITE;
				cmds[j].fileName = ptrArr[i];
			}
			else {
				fprintf(stderr,
					"Unrecognized command: write %s\n", ptrArr[i]);
				ret_val = CMD_PARSE_ERR;
				goto CLEAN_UP;
			}
		}
		else if(strcmp(ptrArr[i], "append") == 0) {
			i++;
			if(strcmp(ptrArr[i], ">>") == 0) {
				i++;
				cmds[j].type = CMD_APPEND;
				cmds[j].fileName = ptrArr[i];
			}
			else {
				fprintf(stderr,
					"Unrecognized command: append %s\n", ptrArr[i]);
				ret_val = CMD_PARSE_ERR;
				goto CLEAN_UP;
			}
		}
		else {
			fprintf(stderr,
						"Unrecognized command: %s\n", ptrArr[i]);
				ret_val = CMD_PARSE_ERR;
				goto CLEAN_UP;
		}
	}

	ret_val = j + 1;	//Return the number of separate commands in the array

	CLEAN_UP:
		return ret_val;
}

#define MAX_CMD_LEN 300
int Command_execute(Command_ts cmd) {
	int ret_val = 0;

	char cmd_str[MAX_CMD_LEN] = {'\0'};

	switch(cmd.type) {
	case CMD_READ:
		strcat(cmd_str, "cp -f ");
		strcat(cmd_str, cmd.fileName);
		strcat(cmd_str, " temp.txt");
		system(cmd_str);
		break;
	case CMD_WRITE:
		strcat(cmd_str, "cp -f ");
		strcat(cmd_str, "temp.txt ");
		strcat(cmd_str, cmd.fileName);
		system(cmd_str);
		break;
	case CMD_APPEND:
		strcat(cmd_str, "cat ");
		strcat(cmd_str, "temp.txt >> ");
		strcat(cmd_str, cmd.fileName);
		system(cmd_str);
		break;
	default:
		fprintf(stderr, "Unknown command type.\n");
		ret_val = CMD_EXEC_ERR;
	}

	return ret_val;
}
