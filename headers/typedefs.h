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
	reg_choise_p,
	guest_choise,
	login_choise,
	login_choise_p,
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
	online_guest_w,
	online_login,
	online_login_w,
	online_login_r,
	online_login_r_w,
	online_admin,
	online_admin_w,
	online_super,
	online_super_w,
	unknown_command,
	good_bye
} _state;

typedef struct connect {
	int32_t fd;
	struct connect *next, *prev;
	_state st;
	char *login;
	char buf[1450];
	char file_position;
	char rights; /*0,1,2,3,4 (0 = su, 1 = a, 2 = u*, 3 = u, 4 = g)*/
} _connect;

#endif
