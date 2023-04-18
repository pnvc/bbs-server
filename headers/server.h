#ifndef SERVER_H_SENTRY
#define SERVER_H_SENTRY

#include <inttypes.h>
#include <signal.h>
#include <syslog.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include "typedefs.h"
#include "server_config.h"
#include "start_deamon_process.h"
#include "signal_handler.h"
#include "working_files.h"
#include "pollfd_control.h"
#include "connections.h"
#include "recv_send_state_logic.h"

#endif
