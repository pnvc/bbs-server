#include "../headers/server_config.h"

int32_t set_config(FILE *config, char **ip, char **port, char **directory,
    char **screen_file, char **accounts_file)
{
    char buf[350];
    char i[] = "IP=";
    char p[] = "PORT=";
    char d[] = "DIRECTORY=";
    char s[] = "SCREEN_FILE=";
    char a[] = "ACCOUNTS_FILE=";
    char lh[] = "127.0.0.1";
    char pd[] = "33333";
    char dd[] = "busy";
    char sd[] = "screen.jpg";
    char ad[] = "accounts";
    *ip = *port = *directory = *screen_file = *accounts_file = NULL;
    while (fgets(buf, sizeof(buf), config)) {
        if (buf[0] == '#' || buf[0] == '\n') {
            continue;
        }
        if (!strncmp(buf, i, sizeof(i) - 1)) {
            if (set_from_config(ip, buf, sizeof(i) - 1) < 0) {
                return -1;
            }
        } else if (!strncmp(buf, p, sizeof(p) - 1)) {
            if (set_from_config(port, buf, sizeof(p) - 1) < 0) {
                return -1;
            }
        } else if (!strncmp(buf, d, sizeof(d) - 1)) {
            if (set_from_config(directory, buf, sizeof(d) - 1) < 0) {
                return -1;
            }
        } else if (!strncmp(buf, s, sizeof(s) - 1)) {
            if (set_from_config(screen_file, buf, sizeof(s) - 1) < 0) {
                return -1;
            }
        } else if (!strncmp(buf, a, sizeof(a) - 1)) {
            if (set_from_config(accounts_file, buf, sizeof(a) - 1) < 0) {
                return -1;
            }
        }
    }
    if (!*ip) {
        if (set_default(ip, lh, sizeof(lh) - 1) < 0) {
            return -1;
        }
    }
    if (!*port) {
        if (set_default(port, pd, sizeof(pd) - 1) < 0) {
            return -1;
        }
    }
    if (!*directory) {
        if (set_default(directory, dd, sizeof(dd) - 1) < 0) {
            return -1;
        }
    }
    if (!*screen_file) {
        if (set_default(screen_file, sd, sizeof(sd) - 1) < 0) {
            return -1;
        }
    }
    if (!*accounts_file) {
        if (set_default(accounts_file, ad, sizeof(ad) - 1) < 0) {
            return -1;
        }
    }
    return 1;
}

int32_t set_from_config(char **dst, char *buf, size_t sparam)
{
    size_t ms;
    size_t n;
    if (*dst) {
        free(*dst);
        *dst = NULL;
    }
    for (n = sparam; buf[n] != ';' && buf[n] != '\n'; n++);
    if (n == sparam || buf[n] == '\n') {
        return 0;
    }
    buf[sparam - 1] = buf[n] = 0;
    ms = n - sparam;
    *dst = (char*)malloc(sizeof(char) * ms);
    if (!memcpy(*dst, buf + sparam, ms)) {
        fprintf(stderr, "memcpy for %s problem", buf);
        return -1;
    }
    return 1;
}

int32_t set_default(char **dst, char *dflt, size_t sdflt)
{
    *dst = (char*)malloc(sizeof(char) * sdflt);
    if (!memcpy(*dst, dflt, sdflt)) {
        fprintf(stderr, "memcpy default %s problem\n", dflt);
        return -1;
    }
    return 1;
}