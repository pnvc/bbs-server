#ifndef TYPEDEFS_H_SENTRY
#define TYPEDEFS_H_SENTRY

#include <inttypes.h>

typedef struct sockaddr sa;
typedef struct sockaddr_in sa_in;

typedef enum state {
	off,
	screen,
	after_screen,
	rgl_choise,
	reg_choise,
	guest_choise,
	login_choise,
	reg_l,
	reg_p,
	reg_bad_l,
	reg_bad_p,
	reg_success,
	login_l,
	login_p,
	login_bad_l,
	login_bad_p,
	login_success,
	online_guest,
	online_login,
} _state;

typedef struct connect {
	int32_t fd;
	struct connect *next, *prev;
	_state st;
	char *login;
	char buf[1450];
	char file_position;
	char rights; /*0,1,2,3 (0 = su, 1 = a, 2 = u, 3 = g)*/
} _connect;

#endif
