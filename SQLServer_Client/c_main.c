#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#define MAX_BUF 256

static const char*
getTime(char* const, size_t);

int main(int argc, char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <ip_address> <port>", argv[0]);
		exit(EXIT_FAILURE);
	}

	int fd = open("a4ClientLog.txt", O_WRONLY | O_CREAT | O_APPEND);
	if(fd < 0) {
		perror("Error opening log file");
		exit(EXIT_FAILURE);
	}

	char buffer[MAX_BUF] = {0};

	int sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sockfd < 0) {
		perror("Error opening socket");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in server_addr = {0};
	server_addr.sin_family = AF_INET;
	if(!inet_aton(argv[1], &server_addr.sin_addr)) {
		perror("Error, invalid ip address");
		exit(EXIT_FAILURE);
	}
	server_addr.sin_port = htons(atoi(argv[2]));

	if(connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr))
												< 0) {
		perror("Error connecting to server");
		exit(EXIT_FAILURE);
	}

	#define TIME_LEN 30
	char timeBuf[TIME_LEN] = {0};

	int contSession = 1;
	while(contSession) {
		memset(buffer, 0, MAX_BUF);
		memset(timeBuf, 0, TIME_LEN);

		printf("Enter sql command.  Enter exit to end session.\n");

		if(fgets(buffer, MAX_BUF, stdin) == NULL)
			break;
		size_t len = strlen(buffer) - 1;
		buffer[len] = '\0';

		if(!(contSession = strcmp(buffer, "exit")))
			break;

		if(send(sockfd, buffer, len, 0) < 0) {
			perror("Error writing to socket");
			break;
		}

		dprintf(fd, "%s\tPID: %d\tCommand: %s\n",
			getTime(timeBuf, TIME_LEN), getpid(), buffer);

		int r;
		if((r = recv(sockfd, buffer, MAX_BUF - 1, 0)) <= 0) {
			if(r < 0)
				perror("Error reading from socket");
			else
				fprintf(stdout, "Connection terminated by server\n");
			break;
		}

		fprintf(stdout, "%s\n%s\n", timeBuf, buffer);
		dprintf(fd, "%s\n%s\n", timeBuf, buffer);
	}

	close(fd);
	close(sockfd);
	exit(EXIT_SUCCESS);
}

static const char*
getTime(char* const timeBuf, size_t len) {
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	strftime(timeBuf, len, "%c", timeinfo);
	return timeBuf;
}
