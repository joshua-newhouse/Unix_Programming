#include <stdio.h>
#include <string.h>

#define BUF_SIZE 300
#define MAX_PTRS 5

/*The type of lexeme:
	LEX_CMD is a command
	LEX_OPT are options
	LEX_ARG is an argument
	LEX_FILE is a file
	LEX_RED is a redirect operator
	LEX_PIPE is the pipe operator
*/
typedef enum {
	LEX_CMD, LEX_OPT, LEX_ARG, LEX_FILE, LEX_RED, LEX_PIPE
} lexemeType_te;

int main(void) {

	/*Create and init buffer for getting user input.
	  Create char array to point at each lexeme of command in buffer
	  ptr_idx is used to index the array of lexemes */
	char buf[BUF_SIZE] = {0};
	char* ptrArr[MAX_PTRS] = {NULL};
	size_t ptr_idx = 0;

	/*Infinite loop.  Loop is terminated when 'exit' command entered by usr */
	while(1) {
		/*Get user input command from stdin and replace \n with \0 */
		printf("> ");
		fgets(buf, BUF_SIZE, stdin);
		size_t bLen = strlen(buf);
		buf[bLen - 1] = '\0';

		/*Check for exit condition */
		if(!strcmp(buf, "exit"))
			break;

		/*Init pointers to begin and end of command string
		  Assign first pointer in array to point at first lexeme in cmd */
		char* s = buf;
		char* end = &buf[bLen-1];
		ptrArr[ptr_idx++] = s;

		/*Move through command string marking the end of each lexeme with 
		  \0 and assigning a pointer in array to point at beginning of each
		  lexeme */
		while(s < end) {
			switch(*s) {
			case ' ':
				*s++ = '\0';
				ptrArr[ptr_idx++] = s;
				break;
			default:
				s++;
			}
		}

		/*Determine type of lexeme and print appropriate message*/
		lexemeType_te type = LEX_CMD;
		size_t i;
		for(i = 0; ptrArr[i] && i < MAX_PTRS; i++) {
			if(*ptrArr[i] == '|') {
				type = LEX_PIPE;
				printf("pipe\n");
			}
			else if(*ptrArr[i] == '>' || *ptrArr[i] == '<') {
				type = LEX_RED;
				printf("File Redirection: %s\n", ptrArr[i]);
			}
			else if(*ptrArr[i] == '-') {
				type = LEX_OPT;
				printf("Options: %s\n", ptrArr[i]);
			}
			else if(!i || type == LEX_PIPE) {
				type = LEX_CMD;
				printf("Command: %s\n", ptrArr[i]);
			}
			else if(type == LEX_OPT || type == LEX_ARG) {
				type = LEX_ARG;
				printf("Argument: %s\n", ptrArr[i]);
			}
			else {
				type = LEX_FILE;
				printf("FILE: %s\n", ptrArr[i]);
			}

			ptrArr[i] = NULL;
		}

		ptr_idx = 0;
	}

	return 0;
}
