#include "../headers/recv_send_state_logic.h"
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syslog.h>

extern char *SCREEN_FILE_BUF;
extern size_t SCREEN_FILE_BUF_len;

extern char *ACCOUNTS_FILE;

static const char noscreen_msg[] = "> No screen file :(\n";
static const char after_screen_msg[] = "> Send one of commands to access: GUEST, LOGIN, REG\n";
static const char command_guest[] = "GUEST\r\n";
static const char command_login[] = "LOGIN\r\n";
static const char command_reg[] = "REG\r\n";
static const char guest_choise_msg[] = "> Welcome, guest. You have next command: LIST, EXIT, DOWNLOAD\n";
static const char reg_choise_msg[] = "> Please enter the login you wish. Beware, case sensetive!\n";
static const char reg_choise_p_msg[] = "> Enter the password you wish\n";
static const char reg_bad_l_msg[] = "> You need more 4 and less 20 symbols or this login is busy\n";
static const char reg_bad_p_msg[] = "> You need more 4 and less 50 symbols\n";
static const char online_login_msg[] = "> You have next commands: LIST, EXIT, DOWNLOAD\n";
static const char login_choise_msg[] = "> Enter loging\n";
static const char login_choise_p_msg[] = "> Enter password\n";
static const char login_bad_msg[] = "This user doesn't exists\n";

static const char unknown_command_msg[] = "> Unknown command, repeat please :)\n";
static const char good_bye_msg[] = "> Good bye :)\n";


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
	char reg_success_msg[37];
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
			c->st = online_guest_w;
			if (send(c->fd, guest_choise_msg, sizeof(guest_choise_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send welcome message for guest to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case reg_choise:
			c->st = reg_l;
			if (send(c->fd, reg_choise_msg, sizeof(reg_choise_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send registration choise message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case reg_choise_p:
			c->st = reg_p;
			if (send(c->fd, reg_choise_p_msg, sizeof(reg_choise_p_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send registration choise password message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case reg_bad_l:
			c->st = reg_l;
			if (send(c->fd, reg_bad_l_msg, sizeof(reg_bad_l_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send registration bad login message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case reg_bad_p:
			c->st = reg_p;
			if (send(c->fd, reg_bad_p_msg, sizeof(reg_bad_p_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send registration bad password message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case reg_success:
			c->rights = 3;
			c->st = online_login;
			sprintf(reg_success_msg, "> Welcome, dear %s\n", c->login);
			if (send(c->fd, (const char*)reg_success_msg, strlen(reg_success_msg), 0) < 0) {
				syslog(LOG_CRIT, "Unable to send registration success message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_choise:
			c->st = login_l;
			if (send(c->fd, login_choise_msg, sizeof(login_choise_msg), 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login choise message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_choise_p:
			c->st = login_p;
			if (send(c->fd, login_choise_p_msg, sizeof(login_choise_msg), 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login choise password message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case online_login:
			c->st = online_login_w;
			if (send(c->fd, online_login_msg, sizeof(online_login_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send online login message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
			}
			break;
/* -------------------------------------------------------------------- */
		case unknown_command:
			if (!c->login) {
				c->st = rgl_choise;
			} else if (c->login == login_guest) {
				c->st = online_guest_w;
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

void check_recv_from_tmp_and_change_state(_connect *c, char *buf)
{
	FILE *fa;
	char buf_read_account_file[74];
	size_t buf_read_account_file_length = 74;
	size_t new_login_length;
	size_t new_password_length;
	char new_login_password[72] = {0};
	switch (c->st) {
		case rgl_choise:
			if (!strncmp(buf, (const char*)command_guest, 7) && !buf[7]) {
				c->st = guest_choise;
				c->login = login_guest;
			} else if (!strncmp(buf, (const char*)command_login, 7) && !buf[7]) {
				c->st = login_choise;
			} else if (!strncmp(buf, (const char*)command_reg, 5) && !buf[5]) {
				c->st = reg_choise;
			} else {
				c->st = unknown_command;
			}
			break;
		case reg_l:
			c->st = reg_choise_p;
			if (!buf[6] || buf[22]) {
				c->st = reg_bad_l;
			} else {
				fa = fopen((const char*)ACCOUNTS_FILE, "r");
				if (!fa) {
					syslog(LOG_CRIT, "Unable open accounts file for checking login: %s", strerror(errno));
					c->st = off;
				} else {
					while (fgets(buf_read_account_file, sizeof(buf_read_account_file), fa))  {
						if (!(new_login_length = compare_new_login_with_accounts((const char*)buf, (const char*)buf_read_account_file))) {
							c->st = reg_bad_l;
							break;
						}
						memset(buf_read_account_file, 0, buf_read_account_file_length);
					}
					if (c->st == reg_choise_p) {
						if (make_connect_login(&(c->login), (const char*)buf, new_login_length) < 0) {
							syslog(LOG_CRIT, "Unable create login: %s", strerror(errno));
							c->st = off;
						}
					}
					fclose(fa);
				}
			}
			break;
		case reg_p:
			c->st = reg_success;
			if (!buf[6] || buf[52]) {
				c->st = reg_bad_p;
			} else {
				fa = fopen((const char*)ACCOUNTS_FILE, "a");
				if (!fa) {
					syslog(LOG_CRIT, "Unable open accounts file for add user: %s", strerror(errno));
					c->st = off;
				} else {
					for (new_password_length = 0; buf[new_password_length] != '\r'; new_password_length++);
					buf[new_password_length] = 0;
					sprintf(new_login_password, "%s %s\n", c->login, buf);
					fwrite((const char*)new_login_password, sizeof(char), strlen(new_login_password), fa);
					fclose(fa);
				}
			}
			break;
		case login_l:
			c->st = login_choise_p;
			break;
		case login_p:
			c->st = login_choise;
			break;
		case online_guest_w:
			c->st = guest_choise;
			break;
		case online_login_w:
			c->st = online_login;
			break;
		default:
			break;
	}
}

size_t compare_new_login_with_accounts(const char *new_login, const char *buf_read_account_file)
{
	size_t i;
	for (i = 4; new_login[i] != '\r'; i++);
	if (!strncmp((const char*)new_login, (const char*)buf_read_account_file, i)) {
		if (buf_read_account_file[i] == ' ') {
			return 0;
		}
	}
	return i;
}

int32_t make_connect_login(char **connect_login, const char *temp_buf_login, size_t new_login_length)
{
	*connect_login = NULL;
	*connect_login = (char*)malloc(sizeof(char) * new_login_length + 1);
	if (!*connect_login) {
		return -1;
	}
	memcpy(*connect_login, temp_buf_login, new_login_length);
	(*connect_login)[new_login_length] = 0;
	return 0;
}
