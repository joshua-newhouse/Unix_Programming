CC = g++
FLAGS = -Wall
OBJ = main.o CLI.o Command.o
HDRS = CLI.h Command.h
PROG = a3part3

default: clean all

all: $(OBJ)
	$(CC) -o $(PROG) $(FLAGS) $(OBJ)

main: main.cpp $(HDRS)
	$(CC) $(FLAGS) -c main.cpp

CLI: CLI.cpp CLI.h
	$(CC) $(FLAGS) -c CLI.cpp

Command: Command.cpp Command.h
	$(CC) $(FLAGS) -c command.cpp

clean:
	rm -f $(PROG) *.o
