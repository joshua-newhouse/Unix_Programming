#include <stdio.h>
#include <stdlib.h>

#include "Server.h"
#include "Sql.h"

int
main(int argc, char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "Usage: %s <ip_address> <port>", argv[0]);
		exit(EXIT_FAILURE);
	}

	Sql_init("company", NULL, (size_t*)0);

	Server_ts server = {0};

	if(Server_init(&server, argv[1], argv[2], "a4ServerLog.txt"))
		exit(EXIT_FAILURE);

	if(Server_run(&server))
		exit(EXIT_FAILURE);

	Server_close(&server);
	exit(EXIT_SUCCESS);
}
