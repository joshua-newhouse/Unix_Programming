#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

#include "Server.h"
#include "Sql.h"

#define MAX_BUF 256
static char buffer[MAX_BUF];
static void* task(void*);

int
Server_init(Server_ts* server, const char* const ip, const char* const port,
		const char* const fileName) {
	size_t len = strlen(ip);
	server->ip_address = (char*)calloc(len + 1, sizeof(char));
	memcpy(server->ip_address, ip, len + 1);

	server->port_number = atoi(port);

	server->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(server->sockfd < 0) {
		perror("Error opening socket");
		return -1;
	}

	server->server_addr.sin_family = AF_INET;
	if(!inet_aton(server->ip_address, &server->server_addr.sin_addr)) {
		perror("Invalid ip address");
		return -1;
	}
	server->server_addr.sin_port = htons(server->port_number);

	if(bind(server->sockfd, (struct sockaddr*)&server->server_addr,
							sizeof(server->server_addr)) < 0) {
		perror("Error on binding");
		return -1;
	}

	server->fd = creat(fileName, O_WRONLY);
	if(server->fd < 0) {
		perror("Error opening log file");
		return -1;
	}

	return 0;
}

int
Server_run(Server_ts* server){
	#define BACKLOG 5
	listen(server->sockfd, BACKLOG);

	int c_sockfd;
	struct sockaddr_in client = {0};
	socklen_t c_len = sizeof(client);

	while(1) {
		c_sockfd = accept(server->sockfd, (struct sockaddr*)&client,
												&c_len);
		if(c_sockfd < 0) {
			perror("Error on accept");
			return -1;
		}
		else {
			printf("Connection accepted\n");
			server->connections++;
			int args[] = {c_sockfd, server->fd};
			pthread_create(&server->threadID, NULL, task, (void*)args);
		}
	}

	return 0;
}

void
Server_close(Server_ts* server) {
	free(server->ip_address);
	close(server->sockfd);
	close(server->fd);
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

int
callback(void* args, int cols, char* entry[], char* header[]) {
	int fd = (int)args;

	int i;
	for(i = 0; i < cols; i++)
		dprintf(fd, "%s\t", header[i]);

	dprintf(fd, "\n");

	for(i = 0; i < cols; i++)
		dprintf(fd, "%s\t", entry[i]);

	dprintf(fd, "\n");

	return 0;
}

static void*
task(void* arg) {
	int* arr = (int*)arg;
	int c_sockfd = arr[0];
	int fd = arr[1];

	#define TIME_LEN 30
	char timeBuf[TIME_LEN] = {0};

	size_t len = 0;
	int serverRun = 1;
	while(serverRun) {
		memset(buffer, 0, MAX_BUF);
		memset(timeBuf, 0, TIME_LEN);

		if(recv(c_sockfd, buffer, MAX_BUF - 1, 0) <= 0) {
			if(errno)
				perror("Error reading from socket");
			break;
		}

		printf("%s\tPID: %d\tThreadID: %lu\tCommand: %s\n",
			getTime(timeBuf, TIME_LEN), getpid(), pthread_self(), buffer);
		dprintf(fd, "%s\tPID: %d\tThreadID: %lu\tCommand: %s\n",
			getTime(timeBuf, TIME_LEN), getpid(), pthread_self(), buffer);

		if(!(serverRun = strcmp(buffer, "shutdown")))
			break;

		Sql_command(buffer, callback, (void*)c_sockfd);

		len = strlen(buffer);
		if(send(c_sockfd, buffer, len, 0) < 0) {
			perror("Error writing to socket");
			break;
		}
	}

	if(errno)
		pthread_exit((void*)EXIT_FAILURE);
	else if(!serverRun)
		exit(EXIT_SUCCESS);
	else
		pthread_exit((void*)EXIT_SUCCESS);
}
