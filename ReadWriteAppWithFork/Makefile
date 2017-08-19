CC = gcc
FLAGS = -Wall -Wno-unused-result
PROGRAM = CommandsWithFork
OBJECTS = program.o command.o

default: $(OBJECTS)
	$(CC) $(FLAGS) -o $(PROGRAM) $(OBJECTS)

program: program.c
	$(CC) $(FLAGS) -c program.c

command: command.c
	$(CC) $(FLAGS) -c command.c

clean:
	rm -f $(PROGRAM) *.o
