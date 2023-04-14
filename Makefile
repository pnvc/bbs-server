MAINFILE=server.c
FOREXECUTE=$(MAINFILE:.c=)
MDIR=modules/
HDIR=headers/
CFLAGS=-Wall -g -ansi -pedantic
CC=gcc
OBJMODULES=$(MDIR)server_config.o $(MDIR)start_deamon_process.o $(MDIR)signal_handler.o $(MDIR)working_files.o $(MDIR)pollfd_control.o $(MDIR)connections.o $(MDIR)recv_send_state_logic.o
OBJHEADERS=$(HDIR)server_config.h $(HDIR)start_deamon_process.h $(HDIR)signal_handler.h $(HDIR)working_files.h $(MDIR)pollfd_control.h $(HDIR)connections.h $(HDIR)recv_send_state_logic.h

#MAINFILE_CLIENT=client.c
#FOREXECUTE_CLIENT=$(MAINFILE_CLIENT:.c=)
#MDIR_CLIENT=client_modules/
#HDIR_CLIENT=client_headers/
#OBJMODULES_CLIENT=
#OBJHEADERS_CLIENT=

%.o:%.c %.h
	$(CC) $(CFLAGS) -c $< -o $@

compilation:$(MAINFILE) $(OBJMODULES)
	$(CC) $(CFLAGS) $^ -o $(FOREXECUTE)

#compilation_client:$(MAINFILE_CLIENT) $(OBJMODULES_CLIENT)
#	$(CC) $(CFLAGS) $^ -o $(FOREXECUTE_CLIENT)

run:server
	./server

clean:
	rm $(MDIR)*.o
