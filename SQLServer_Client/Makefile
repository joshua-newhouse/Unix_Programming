CC = gcc
CFLAGS = -Wall -pedantic
S_OBJ = s_main.o Server.o Sql.o
SERVER = server
C_OBJ = c_main.o
CLIENT = client

all: $(SERVER) $(CLIENT)

$(SERVER): $(S_OBJ)
	$(CC) -o $(SERVER) $(S_OBJ) -lpthread -lsqlite3

s_main: s_main.c Server.h
	$(CC) $(CFLAGS) s_main.c

Server: Server.c Server.h
	$(CC) $(CFLAGS) Server.c

Sql: Sql.c Sql.h
	$(CC) $(CFLAGS) Sql.c

$(CLIENT): $(C_OBJ)
	$(CC) -o $(CLIENT) $(C_OBJ)

c_main: c_main.c
	$(CC) $(CFLAGS) c_main.c

clean:
	rm -f server client *.o
