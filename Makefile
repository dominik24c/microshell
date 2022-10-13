CC=gcc 
CFLAGS=-ansi -Wall
FILENAME=microshell

all: 
	$(CC) $(CFLAGS) -o $(FILENAME) $(FILENAME).c
 
clean: 
	rm $(FILENAME)
