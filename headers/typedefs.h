#ifndef TYPEDEFS_H_SENTRY
#define TYPEDEFS_H_SENTRY

#include <inttypes.h>

typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

typedef enum state {
  off,
  screen,
  reg,
  guest,
  login,
  online
} _state;

typedef struct connect {
  int32_t fd;
  struct connect *next, *prev;
  _state st;
  char *login;
  char buf[1450];
  char rights; /*0,1,2,3 (0 = su, 1 = a, 2 = u, 3 = g)*/
} _connect;

#endif
