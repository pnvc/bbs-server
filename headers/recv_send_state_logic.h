#ifndef RECV_SEND_STATE_LOGIC_H_SENTRY
#define RECV_SEND_STATE_LOGIC_H_SENTRY

#include "typedefs.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>

#ifndef NULL
#define NULL (void*)0
#endif

_connect *comparison_pollfd_with_connect(_connect *f, const int32_t pollfd_fd);
#if 1
void send_to_tmp_and_change_state(_connect *c);
#endif

#endif
