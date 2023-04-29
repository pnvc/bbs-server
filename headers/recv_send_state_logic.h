#ifndef RECV_SEND_STATE_LOGIC_H_SENTRY
#define RECV_SEND_STATE_LOGIC_H_SENTRY

#include "typedefs.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <syslog.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <sys/syslog.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#ifndef NULL
#define NULL (void*)0
#endif

_connect *comparison_pollfd_with_connect(_connect *f, const int32_t pollfd_fd);
void send_to_tmp_and_change_state(_connect *c);
void check_recv_from_tmp_and_change_state(_connect *c, char *buf);
size_t compare_new_login_with_accounts(const char *new_login, const char *buf_read_account_file);
int32_t make_connect_login(char **connect_login, const char *temp_buf_login, size_t new_login_length);

#endif
