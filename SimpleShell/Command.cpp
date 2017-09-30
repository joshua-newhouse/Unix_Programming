#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>


#include "Command.h"

namespace Command {
	/*Takes a command structure and an array of char* and produces an array of
	  strings in argv. */
	void setUpArgs(Command_ts cmd, const char** argv) {
		size_t size = cmd.args.size();
		for(size_t i = 0; i < size; i++) {
			argv[i] = cmd.args[i].c_str();
		}
		argv[size] = (char*)0;
	}

	/*Takes an array of command strings and the number of command strings
	  (elem) and forks off processes to execute the commands */
	void process(std::string commands[], size_t elem) {
		std::vector<Command_ts> comVec;

		/*Breaks each command string into a vector of argument strings and
		  stores them in a command structure that is then added to a vector
		  of command structures. */
		for(size_t i = 0; i < elem; i++) {
			if(commands[i][0] == ' ')
				commands[i].erase(0, 1);

			std::istringstream cmdString(commands[i]);

			Command_ts temp = Command_ts();
			std::string arg;
			while(std::getline(cmdString, arg, ' ')) {
				if(arg[0] == '>') {
					if(arg[1] == '>')
						temp.redirection = Command::RD_APPEND;
					else
						temp.redirection = Command::RD_OUT;
				}
				else if(arg[0] == '<') {
					temp.redirection = Command::RD_IN;
				}
				else {
					if(temp.redirection)
						temp.fileName = arg;
					else
						temp.args.push_back(arg);
				}
			}

			comVec.push_back(temp);
		}

		/*For each command in the vector of command structures, creates a
		  pipe if there is a following command and then forks and exec on
		  the current command. */
		size_t n = comVec.size();
		int i, prev_pipe_read = STDIN_FILENO;
		for(i = 0; i < n; i++) {
			int pipeCreated = 0;
			int pipe_fd[2] = {STDIN_FILENO, STDOUT_FILENO};

			/*Create a pipe if the current command is not the last and
			  the current command does not have a write or append
			  redirection*/
			Redirection_te rd = comVec[i].redirection;
			if(i + 1 < n && rd != RD_OUT && rd != RD_APPEND) {
				pipe(pipe_fd);
				pipeCreated = 1;
			}

			if(fork() == 0) { //Child process
				/*If a pipe was created then write to it;
				  else if the command has an out or append redirect
				  open the file and map the STDOUT to it;
				  else write to the STDOUT by default */
				if(pipeCreated) {
					dup2(pipe_fd[1], STDOUT_FILENO);
					close(pipe_fd[1]);
				}
				else if(rd == RD_OUT) {
					int fd = creat(comVec[i].fileName.c_str(),
											S_IRWXU);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}
				else if(rd == RD_APPEND) {
					int fd = open(comVec[i].fileName.c_str(),
									O_WRONLY | O_APPEND);
					dup2(fd, STDOUT_FILENO);
					close(fd);
				}

				/*If the command has an input redirect open the file and
				  map it to the STDIN;
				  else if a pipe was created before this command
				  (by a previous command) then read from it;
				  else read from STDIN by default*/
				if(rd == RD_IN) {
					int fd = open(comVec[i].fileName.c_str(),
									O_RDONLY);
					dup2(fd, STDIN_FILENO);
					close(fd);
				}
				else if(prev_pipe_read != STDIN_FILENO) {
					dup2(prev_pipe_read, STDIN_FILENO);
					close(prev_pipe_read);
				}

				//Format the command into an array of char* and exec it
				size_t size = comVec[i].args.size();
				const char** argv = new const char*[size + 1]();
				setUpArgs(comVec[i], argv);
				execvp(argv[0], (char* const*)argv);
			}
			else { //Parent
				/*If a pipe was created close the write end and set
				  previous_pipe_read to the read end */
				if(pipeCreated) {
					if(prev_pipe_read != STDIN_FILENO)
						close(prev_pipe_read);
					prev_pipe_read = pipe_fd[0];
					close(pipe_fd[1]);
				}
			}
		}

		for(i = 0; i < n; i++) {
			int status;
			wait(&status);
		}
	}

	/*Prints a command structure.  Useful for troubleshooting*/
	void print(Command_ts cmd) {
		size_t n = cmd.args.size();
		for(int i = 0; i < n; i++)
			if(cmd.args[i] != "")
				std::cout << cmd.args[i] << std::endl;

		std::string out;
		switch(cmd.redirection) {
		case Command::RD_NONE:
			out = "None";
			break;
		case Command::RD_IN:
			out = "Input " + cmd.fileName;
			break;
		case Command::RD_OUT:
			out = "Output " + cmd.fileName;
			break;
		case Command::RD_APPEND:
			out = "Append " + cmd.fileName;
			break;
		default:
			break;
		}

		std::cout << "Redirection: " << out << std::endl;
	}
}
