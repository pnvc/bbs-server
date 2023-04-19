#ifndef RECV_SEND_STATE_LOGIC_H_SENTRY
#define RECV_SEND_STATE_LOGIC_H_SENTRY

#include "typedefs.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <inttypes.h>

#ifndef NULL
#define NULL (void*)0
#endif

_connect *comparison_pollfd_with_connect(_connect *f, const int32_t pollfd_fd);
void send_to_tmp_and_change_state(_connect *c);
void check_recv_from_tmp_and_change_state(_connect *c, const char *buf);
int32_t compare_new_login_with_accounts(const char *new_login, const char *accounts);
int32_t make_connect_login(char *connect_login, const char *temp_buf_login);

#endif
