MAINFILE=server.c
FOREXECUTE=$(MAINFILE:.c=)
MDIR=modules/
HDIR=headers/
CFLAGS=-Wall -g -ansi -pedantic
CC=gcc
OBJMODULES=$(MDIR)server_config.o $(MDIR)start_deamon_process.o $(MDIR)signal_handler.o $(MDIR)working_files.o
OBJHEADERS=$(HDIR)server_config.h $(HDIR)start_deamon_process.h $(HDIR)signal_handler.h $(HDIR)working_files.h

%.o:%.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

compilation:$(MAINFILE) $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $(FOREXECUTE)

run:server
	./server

clean:
	rm $(MDIR)*.o
