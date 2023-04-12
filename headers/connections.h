#ifndef CONNECTIONS_H_SENTRY
#define CONNECTIONS_H_SENTRY

#include "typedefs.h"
#include <string.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <syslog.h>
#include <sys/socket.h>
#include <poll.h>

int32_t create_connect(_connect **f, _connect **l, const int32_t fd);
void close_and_remove_off_connections(_connect **f, _connect **l, struct pollfd *pfds, const size_t pfdli);
void close_and_remove_all_connections(_connect **f, _connect **l);

#endif
