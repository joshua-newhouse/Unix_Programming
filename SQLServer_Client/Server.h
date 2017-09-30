#ifndef SERVER_H
#define SERVER_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>

typedef struct Server {
	char* ip_address;
	unsigned short port_number;
	struct sockaddr_in server_addr;
	int sockfd;
	size_t connections;
	pthread_t threadID;
	int fd;
} Server_ts;

int
Server_init(Server_ts*,
		const char* const,
		const char* const,
		const char* const);

int
Server_run(Server_ts*);

void
Server_close(Server_ts*);

#endif
