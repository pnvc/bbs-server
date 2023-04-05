MAINFILE=server.c
FOREXECUTE=$(MAINFILE:.c=)
MODULESDIR=modules/
HEADERSDIR=headers/
CFLAGS=-Wall -g -ansi -pedantic
CC=gcc
OBJMODULES=$(MODULESDIR)server_config.o
OBJHEADERS=$(HEADERSDIR)server_config.h

%.o:%.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

compilation:$(MAINFILE) $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $(FOREXECUTE)

run:server
	./server

clean:
	rm $(MODULESDIR)*.o
