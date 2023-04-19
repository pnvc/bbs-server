#include "../headers/recv_send_state_logic.h"
#include <string.h>
#include <sys/syslog.h>

extern char *SCREEN_FILE_BUF;
extern size_t SCREEN_FILE_BUF_len;

static const char noscreen_msg[] = "No screen file :(\n";
static const char after_screen_msg[] = "Send one of commands to access: GUEST, LOGIN, REG\n";
static const char command_guest[] = "GUEST\r\n";
static const char command_login[] = "LOGIN\r\n";
static const char command_reg[] = "REG\r\n";
static const char guest_choise_msg[] = "Welcome, guest. You have next command: LIST, EXIT, DOWNLOAD\n";
static const char unknown_command_msg[] = "Unknown command, repeat please :)\n";
static const char good_bye_msg[] = "Good bye :)\n";

char login_guest[] = "Guest";

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
		case guest_choise:
			c->st = online_guest;
			if (send(c->fd, guest_choise_msg, sizeof(guest_choise_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send welcome message for guest ro %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case unknown_command:
			if (!c->login) {
				c->st = rgl_choise;
			} else if (c->login == login_guest) {
				c->st = online_guest;
			} else {
				c->st = online_login;
			}
			if (send(c->fd, unknown_command_msg, sizeof(unknown_command_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send unknown command message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case good_bye:
			c->st = off;
			if (send(c->fd, good_bye_msg, sizeof(good_bye_msg) - 1, 0) < 0) {
				syslog(LOG_WARNING, "Unable to send good bye message for user to %d socket: %s", c->fd, strerror(errno));
			}
			break;
		default:
			break;
	}
}

void check_recv_from_tmp_and_change_state(_connect *c, const char *buf)
{
	switch (c->st) {
		case rgl_choise:
			if (!strncmp(buf, (const char*)command_guest, 7)) {
				c->st = guest_choise;
				c->login = login_guest;
			} else if (!strncmp(buf, (const char*)command_login, 7)) {
				c->st = login_choise;
			} else if (!strncmp(buf, (const char*)command_reg, 5)) {
				c->st = reg_choise;
			} else {
				c->st = unknown_command;
			}
			break;
		default:
			break;
	}
}
