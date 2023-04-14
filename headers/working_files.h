#ifndef WORKING_FILES_H_SENTRY
#define WORKING_FILES_H_SENTRY

#include <inttypes.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <syslog.h>
#include <string.h>
#include <stdlib.h>

int32_t create_working_directory_and_chdir(const char *dir);
int32_t create_accounts_file(const char *acc);
int32_t check_image(const char *img);
char *read_screen_file(const char *screen_file);

#endif
