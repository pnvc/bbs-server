cmake_minimum_required(VERSION 3.2)

project(Server)

set(SRC
	src/server.c
	src/connections.c
	src/pollfd_control.c
	src/recv_send_state_logic.c
	src/server_config.c
	src/signal_handler.c
	src/start_deamon_process.c
	src/working_files.c
)

set(HEADERS
	headers/connections.h
	headers/pollfd_control.h
	headers/recv_send_state_logic.h
	headers/server.h
	headers/server_config.h
	headers/signal_handler.h
	headers/start_deamon_process.h
	headers/typedefs.h
	headers/working_files.h
)

add_executable (exe ${SRC} ${HEADERS})

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -ansi -pedantic")
