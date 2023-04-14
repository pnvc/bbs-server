#include "../headers/recv_send_state_logic.h"
#include <string.h>
#include <sys/syslog.h>

extern char *SCREEN_FILE_BUF;
extern size_t SCREEN_FILE_BUF_len;

#if 0
static const char noscreen_msg[] = "No screen file :(\n";
static const char after_screen_msg[] = "Send one of commands to access: GUEST, LOGIN, REG\n";
#endif

_connect *comparison_pollfd_with_connect(_connect *f, const int32_t pollfd_fd)
{
	while (f) {
		if (f->fd == pollfd_fd) {
			return f;
		}
		f = f->next;
	}
	return NULL;
}
#if 0
void send_to_tmp_and_change_state(_connect *c)
{
	switch (c->st) {
		case screen:
			c->st = after_screen;
			if (SCREEN_FILE_BUF) {
				if (send(c->fd, SCREEN_FILE_BUF, SCREEN_FILE_BUF_len, 0) < 0) {
					syslog(LOG_INFO, "Unable to send screen to %d socket: %s", c->fd, strerror(errno));
				}
			} else {
				if (send(c->fd, noscreen_msg, sizeof(noscreen_msg) - 1, 0) < 0) {
					syslog(LOG_INFO, "Unable to send screen to %d socket: %s", c->fd, strerror(errno));
				}
			}
			break;
		case after_screen:
			c->st = rgl_choise;
			if (send(c->fd, after_screen_msg, sizeof(after_screen_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send welcome message to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		default:
			break;
	}
}
#endif
