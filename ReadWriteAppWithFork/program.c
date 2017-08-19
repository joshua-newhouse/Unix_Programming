#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#include "command.h"

#define BUF_SIZE 300

void childTask(char*);

int main(void) {
	pid_t pid;

	/*Create and init buffer for getting user input.*/
	char buf[BUF_SIZE] = {0};

	/*Infinite loop.  Loop is terminated when 'exit' command entered by usr */
	while(1) {
		/*Get user input command from stdin and replace \n with \0 */
		printf("> ");
		fgets(buf, BUF_SIZE, stdin);
		size_t bLen = strlen(buf);
		buf[bLen - 1] = '\0';

		printf("The input command is:\t%s\n", buf);	//Echo the command

		/*Check for exit condition */
		if(!strcmp(buf, "exit"))
			break;

		if((pid = fork()) < 0) {
			fprintf(stderr, "Fork error\n");
			continue;
		}

		if(pid == 0) {
			childTask(buf);
			break;
		}
		else
			waitpid(pid, NULL, 0);
	}

	exit(0);
}

#define MAX_CMDS 3
void childTask(char* buf) {
	Command_ts commands[MAX_CMDS] = {0};

	int cmds = 0;
	if((cmds = Command_parse(buf, commands, MAX_CMDS))
										== CMD_PARSE_ERR) {
		fprintf(stderr, "Error parsing command\n");
		return;
	}

	size_t i;
	for(i = 0; i < cmds && i < MAX_CMDS; i++)
		if(Command_execute(commands[i]))
			fprintf(stderr, "Error executing command\n");
}
