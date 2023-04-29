#include <stdio.h>
#define _XOPEN_SOURCE 500 /* for the snprintf */
#include "../headers/recv_send_state_logic.h"

extern char *SCREEN_FILE_BUF;
extern size_t SCREEN_FILE_BUF_len;

extern char *ACCOUNTS_FILE;

static const char noscreen_msg[] = "> No screen file :(\n";
static const char after_screen_msg[] = "> Send one of commands to access: GUEST, LOGIN, REG\n";
static const char command_guest[7] = "GUEST\r\n";
static const char command_login[7] = "LOGIN\r\n";
static const char command_reg[5] = "REG\r\n";
#if 0
static const char command_list[6] = "LIST\r\n";
static const char command_download[10] = "DOWNLOAD\r\n";
static const char command_chngfileconf[14] = "CHNGFILECONF\r\n";
static const char command_rmfile[8] = "RMFILE\r\n";
static const char command_chngfileconfusr[17] = "CHNGFILECONFUSR\r\n";
static const char command_rmfileusr[11] = "RMFILEUSR\r\n";
static const char command_addusr[8] = "ADDUSR\r\n";
static const char command_rmusr[7] = "RMUSR\r\n";
static const char command_checkmsg[10] = "CHECKMSG\r\n";
static const char command_rmmsg[7] = "RMMSG\r\n";
static const char command_rmadmin[9] = "RMADMIN\r\n";
#endif
static const char command_upload[8] = "UPLOAD\r\n";

static const char guest_choise_msg[] = "> Welcome, guest. You have next command: LIST, EXIT, DOWNLOAD\n";
static const char reg_choise_msg[] = "> Please enter the login you wish. Beware, case sensetive!\n";
static const char reg_choise_p_msg[] = "> Enter the password you wish\n";
static const char reg_bad_l_msg[] = "> You need more 4 and less 20 symbols or this login is busy\n";
static const char reg_bad_p_msg[] = "> You need more 4 and less 50 symbols\n";
static const char online_login_msg[] = "> You have next commands: LIST, EXIT, DOWNLOAD\n";
static const char login_choise_msg[] = "> Enter loging\n";
static const char login_spaces_msg[] = "> No spaces, please\n";
static const char login_short_or_long_l_msg[] = "> You need more than 4 symbols for login and lower than 20\n";
static const char login_short_or_long_p_msg[] = "> You need more than 4 symbols for password and lower than 52\n";
static const char login_choise_p_msg[] = "> Enter password\n";
static const char login_bad_msg[] = "> This user doesn't exists\n";
static const char online_login_r_msg[] = "> You have next commands: LIST, EXIT, DOWNLOAD, UPLOAD, CHNGFILECONF, RMFILE\n";
static const char online_admin_msg[] = "> You have next commands:\n> LIST, EXIT, DOWNLOAD, UPLOAD\n> CHNGFILECONF, RMFILE\n\
> CHNGFILECONFUSR, RMFILEUSR\n> ADDUSR, RMUSR\n";
static const char online_super_msg[] = "> You have next commands:\n> LIST, EXIT, DOWNLOAD, UPLOAD\n> CHNGFILECONF, RMFILE\n\
> CHNGFILECONFUSR, RMFILEUSR\n> ADDUSR, RMUSR\n\
> CHECKMSG, RMMSG\n\
> RMADMIN\n";
static const char upload_config_msg[] = "> Enter the file name and access for users separated by commas\n\
> just file name and * if for all\n\
> just file name if only for you, examples:\n\
> \"File user1,user2,...\"\n\
> \"File *\"\n\
> \"File\"\n";
static const char upload_config_error_msg[] = "> Error with file name length (4 symbols min, 25 symbols max) or this file exists, try again\n";
static const char upload_choose_your_file_msg[] = "> Enter the file name you want to download :)\n";
static const char upload_succes_msg[] = "> Upload success!";
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
	char success_msg[37];
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
			sprintf(success_msg, "> Welcome, dear %s\n", c->login);
			if (send(c->fd, (const char*)success_msg, strlen(success_msg), 0) < 0) {
				syslog(LOG_CRIT, "Unable to send registration success message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_choise:
			c->st = login_l;
			if (send(c->fd, login_choise_msg, sizeof(login_choise_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login choise message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_spaces:
			if (c->login) {
				c->st = login_p;
			} else {
				c->st = login_l;
			}
			if (send(c->fd, login_spaces_msg, sizeof(login_spaces_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login choise message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_short_or_long_l:
			c->st = login_l;
			if (send(c->fd, login_short_or_long_l_msg, sizeof(login_short_or_long_l_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login short or long message for user to %d socket: %s", c->fd, strerror(errno));
			}
			break;
		case login_short_or_long_p:
			c->st = login_p;
			if (send(c->fd, login_short_or_long_p_msg, sizeof(login_short_or_long_p_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login short or loing password message for user to %d socket: %s", c->fd, strerror(errno));
			}
			break;
		case login_choise_p:
			c->st = login_p;
			if (send(c->fd, login_choise_p_msg, sizeof(login_choise_p_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login choise password message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_bad:
			c->st = login_choise;
			free(c->login);
			c->login = NULL;
			if (send(c->fd, login_bad_msg, sizeof(login_bad_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login bad message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case login_success:
			if (c->rights == 3) {
				c->st = online_login;
			} else if (c->rights == 2) {
				c->st = online_login_r;
			} else if (c->rights == 1) {
				c->st = online_admin;
			} else if (c->rights == 0) {
				c->st = online_super;
			}
			sprintf(success_msg, "> Hello, %s\n", c->login);
			if (send(c->fd, (const char*)success_msg, strlen(success_msg), 0) < 0) {
				syslog(LOG_CRIT, "Unable to send login success message for user to %d socket: %s", c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case online_login:
			c->st = online_login_w;
			if (send(c->fd, online_login_msg, sizeof(online_login_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send online login message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case online_login_r:
			c->st = online_login_r_w;
			if (send(c->fd, online_login_r_msg, sizeof(online_login_r_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send online login rights message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case online_admin:
			c->st = online_admin_w;
			if (send(c->fd, online_admin_msg, sizeof(online_admin_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send online login admin message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case online_super:
			c->st = online_super_w;
			if (send(c->fd, online_super_msg, sizeof(online_super_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send online login super message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case upload_config:
			c->st = upload_config_w;
			if (send(c->fd, upload_config_msg, sizeof(upload_config_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send upload config message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case upload_config_error:
			c->st = upload_config_w;
			if (send(c->fd, upload_config_error_msg, sizeof(upload_config_error_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send upload config error message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case upload_choose_your_file:
			c->st = upload_cyf_w;
			if (send(c->fd, upload_choose_your_file_msg, sizeof(upload_choose_your_file_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send upload choose your file message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
		case upload_success:
			switch (c->rights) {
				case 0:
					c->st = online_super;
					break;
				case 1:
					c->st = online_admin;
					break;
				case 2:
					c->st = online_login_r;
					break;
			}
			if (send(c->fd, upload_succes_msg, sizeof(upload_succes_msg) - 1, 0) < 0) {
				syslog(LOG_CRIT, "Unable to send upload success message for user %s to %d socket: %s", c->login, c->fd, strerror(errno));
				c->st = off;
			}
			break;
/* -------------------------------------------------------------------- */
		case unknown_command:
			if (!c->login) {
				c->st = rgl_choise;
			} else if (c->login == login_guest) {
				c->st = online_guest_w;
			} else {
				switch (c->rights) {
					case 0:
						c->st = online_super;
						break;
					case 1:
						c->st = online_admin;
						break;
					case 2:
						c->st = online_login_r;
						break;
					case 3:
						c->st = online_login;
						break;
					case 4:
						c->st = guest_choise;
						break;
					default: break;
				}
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
	char buf_read_account_file[76];
	size_t buf_read_account_file_length = 76;
	size_t new_login_length;
	size_t new_password_length;
	char new_login_password[72] = {0};
	uint64_t login_l_buf_length;
	size_t login_pass_length;
	size_t acc_login_length;
	char rs[3];
	size_t nfssp; /* new file separator space (' ') position */
	char *new_file_config;
	char *new_file;
	FILE *nf, *nfc;
	int32_t nfc_fd, nf_fd;
	int32_t for_umask = 0177;
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
						if (buf_read_account_file[0] == '#' || buf_read_account_file[0] == '\n') {
							memset(buf_read_account_file, 0, buf_read_account_file_length);
							continue;
						}
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
			if (!buf[6] || buf[22]) {
				c->st = login_short_or_long_l;
				break;
			}
			if (strchr(buf, ' ') || strchr(buf, '\t')) {
				c->st = login_spaces;
				break;
			}
			c->st = login_choise_p;
			login_l_buf_length = strlen((const char*)buf) - 2; /* - "\r\n" */
			c->login = (char*)malloc(sizeof(char) * (login_l_buf_length + 1));
			if (!c->login) {
				c->st = off;
				syslog(LOG_CRIT, "Unable malloc for LOGIN process: %s", strerror(errno));
			}
			c->login[login_l_buf_length] = 0;
			strncpy(c->login, (const char*)buf, login_l_buf_length);
			break;
		case login_p:
			if (!buf[6] || buf[52]) {
				c->st = login_short_or_long_p;
				break;
			}
			if (strchr(buf, ' ') || strchr(buf, '\t')) {
				c->st = login_spaces;
				break;
			}
			fa = fopen((const char*)ACCOUNTS_FILE, "r");
			if (!fa) {
				syslog(LOG_CRIT, "Unable open accounts file for add user: %s", strerror(errno));
				c->st = off;
			} else {
				c->st = login_bad;
				while(fgets(buf_read_account_file, 74, fa)) {
					if (buf_read_account_file[0] == '#' || buf_read_account_file[0] == '\n') {
						memset(buf_read_account_file, 0, buf_read_account_file_length);
						continue;
					}
					for (acc_login_length = 0; buf_read_account_file[acc_login_length] != ' '; acc_login_length++);
					login_pass_length = strlen((const char*)buf) - 2;
					if (strlen((const char*)c->login) == acc_login_length &&
							!strncmp((const char*)c->login, (const char*)buf_read_account_file, acc_login_length)) {
						if (!strncmp((const char*)buf, (const char*)(buf_read_account_file + acc_login_length + 1), login_pass_length)) {
							if (buf_read_account_file[acc_login_length + login_pass_length + 1] == '\n') {
								c->rights = 3;
								c->st = login_success;
								break;
							}
							strncpy(rs, (const char*)(buf_read_account_file + acc_login_length + login_pass_length + 2), 3);
							if (!strncmp((const char*)rs, "***", 3)) {
								c->rights = 0;
								c->st = login_success;
								break;
							}
							if (!strncmp((const char*)rs, "**", 2)) {
								c->rights = 1;
								c->st = login_success;
								break;
							}
							if (!strncmp((const char*)rs, "*", 1)) {
								c->rights = 2;
								c->st = login_success;
								break;
							}
						}
					}
				}
				fclose(fa);
			}
			break;
		case online_guest_w:
			c->st = guest_choise;
			break;
		case online_login_w:
			c->st = online_login;
			break;
		case online_login_r_w:
			if (!strncmp(buf, command_upload, 8) && !buf[8]) {
				c->st = upload_config;
			} else {
				c->st = unknown_command;
			}
			break;
		case online_admin_w:
			c->st = online_admin;
			break;
		case online_super_w:
			c->st = online_super;
			break;
		case upload_config_w:
			if (!buf[6]) {
				c->st = upload_config_error;
			} else {
				umask(for_umask);
				if (strchr((const char*)buf, ' ')) {
					nfssp = 4;
					while(buf[nfssp] != ' ') {
						++nfssp;
					}
					if (nfssp > 25) {
						c->st = upload_config_error;
					}
					buf[nfssp] = 0;
					new_file_config = (char*)calloc(sizeof(char) * (nfssp + 7), sizeof(char)); /* sizeof(".fconf") = 6 + 1 for the \0 symbol */
					new_file = (char*)calloc(sizeof(char*) * (nfssp + 1), sizeof(char));
					sprintf(new_file_config, "%s.fconf", buf);
					strncpy(new_file, (const char*)buf, nfssp);
					if ((nfc_fd = open((const char*)new_file_config, O_WRONLY | O_CREAT | O_EXCL, 0777)) < 0) {
						if (errno == EEXIST) {
							c->st = upload_config_error;
						} else {
							c->st = off;
							syslog(LOG_CRIT, "Unable to create config file for user %s socket %d: %s", c->login, c->fd, strerror(errno));
						}
						break;
					} else {	
						if (write(nfc_fd, (const char*)(buf + nfssp + 1), strlen(buf + nfssp + 1)) < 0) {
							c->st = off;
							syslog(LOG_CRIT, "Unable to write to the config file for user %s socket %d: %s", c->login, c->fd, strerror(errno));
						}
						close(nfc_fd);
					}
					c->upload_file_name = (char*)calloc(sizeof(char) * (strlen((const char*)new_file) + 1), sizeof(char));
					strcpy(c->upload_file_name, new_file);
					c->st = upload_choose_your_file;
					if ((nf_fd = open((const char*)new_file, O_WRONLY | O_CREAT | O_TRUNC, 0777)) < 0) {
						c->st = off;
						syslog(LOG_CRIT, "Unable to create file for user %s socket %d: %s", c->login, c->fd, strerror(errno));
					}
					close(nf_fd);
				} else {
					;
				}
			}
			break;
		case upload_cyf_w:
			if (!(nf = fopen((const char*)c->upload_file_name, "a"))) {
				syslog(LOG_CRIT, "Unable to open file for uploading user %s socket %d: %s", c->login, c->fd, strerror(errno));
				c->st = off;
				break;
			} else if (!buf[1450]) {
				fwrite((const char*)buf, sizeof(char), strlen((const char*)buf), nf);
				free(c->upload_file_name);
				c->upload_file_name = NULL;
				c->st = upload_success;
			} else {
				fwrite((const char*)buf, sizeof(char), 1450, nf);
				c->st = upload_cyf_w;
			}
			fclose(nf);
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
