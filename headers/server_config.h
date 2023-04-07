#ifndef SERVER_CONFIG_H_SENTRY
#define SERVER_CONFIG_H_SENTRY

#include <stdio.h>
#include <inttypes.h>
#include <string.h>
#include <stdlib.h>

int32_t set_config(FILE *config, char **ip, char **port, char **directory,
    char **screen_file, char **accounts_file);
int32_t set_from_config(char **dst, char *buf, size_t sparam);
int32_t set_default(char **dst, char *dflt, size_t sdflt);

#endif
