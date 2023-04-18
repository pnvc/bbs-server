#define _GNU_SOURCE
#include "headers/server.h"

#define POLLFD_ARR_LENGTH 1000

extern volatile sig_atomic_t loop_is_possible, config_update;	/* signal_handler.c */

char *SCREEN_FILE_BUF;											/* the buf contains screen file */
size_t SCREEN_FILE_BUF_len;

int32_t main()
{
	char *IP, *PORT, *DIRECTORY, *SCREEN_FILE, *ACCOUNTS_FILE;	/* the future strings for configuration */
	int32_t ls;													/* listen socket */
	int32_t ipr;												/* inet_pton return */
	int32_t optval;												/* setsockopt access value */
	int32_t flags;												/* for set listen socket as flags | O_NONBLOCK */
	int32_t consock;											/* connect socket */
	int32_t ppoll_return;										/* ppoll returns */
	sa_in lsaddr;												/* listen socket address */
	sa_in conaddr;												/* connect address */
	socklen_t conaddr_len;										/* connect address length */
	size_t pfdli; 												/* *pollfd last index */
	size_t pfdi;												/* *pollfd index for check in for() loop */
	struct pollfd *pollfd_ptr;									/* the pointer to the pollfd array */
	nfds_t pollfd_count;										/* pollfds in *pollfd_ptr max count */
	sigset_t user_mask;											/* mask for block SIGHUP, SIGUSR1 and SIGPIPE signals */
	sigset_t old_mask;											/* mask to unblock all blocked signals */
	_connect *first_connect;									/* the server has a list of connections, this is first pointer the list */
	_connect *last_connect;										/* the last pointer of the list */
	_connect *tmp;												/* temporarily _connect in main loop for recv or send */
	FILE *config;												/* FILE pointer for the config/server.conf file */
	char buf[1450] = {0};										/* main buffer for recv */
	ssize_t recv_return;										/* recv returns */
	start_deamon_process();
	openlog("bbs-server", LOG_PID, 0);
	signal(SIGUSR1, usr1_handler);
	signal(SIGHUP, hup_handler);
	signal(SIGPIPE, SIG_IGN);
	sigemptyset(&user_mask);
	sigaddset(&user_mask, SIGUSR1);
	sigaddset(&user_mask, SIGHUP);
	sigaddset(&user_mask, SIGPIPE);
	sigprocmask(SIG_SETMASK, &user_mask, &old_mask);
start:
	SCREEN_FILE_BUF = NULL;
	ls = 0;
	conaddr_len = sizeof(conaddr);
	pfdli = 0;
	pollfd_count = POLLFD_ARR_LENGTH;
	pollfd_ptr = (struct pollfd*)malloc(sizeof(*pollfd_ptr) * POLLFD_ARR_LENGTH);
	fill_initial_pollfd_arr(pollfd_ptr, POLLFD_ARR_LENGTH);
	if (!pollfd_ptr) {
		syslog(LOG_CRIT, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	config = fopen("config/server.conf", "r");
	if (!config) {
    	syslog(LOG_CRIT, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (set_config(config, &IP, &PORT, &DIRECTORY, &SCREEN_FILE, &ACCOUNTS_FILE) < 0) {
		syslog(LOG_CRIT, "set config");
		exit(EXIT_FAILURE);
	}
	fclose(config);
	if (create_working_directory_and_chdir(DIRECTORY) < 0) {
    	exit(EXIT_FAILURE);
	}
	if (create_accounts_file((const char*)ACCOUNTS_FILE) < 0) {
        exit(EXIT_FAILURE);
	}
	if (check_image((const char*)SCREEN_FILE) < 0) {
    	syslog(LOG_INFO, "No screen");
	} else {
		SCREEN_FILE_BUF = read_screen_file((const char*)SCREEN_FILE);
		SCREEN_FILE_BUF_len = strlen((const char*)SCREEN_FILE_BUF);
	}
	fill_initial_pollfd_arr(pollfd_ptr, POLLFD_ARR_LENGTH);
	first_connect = last_connect = NULL;
	ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls < 0) {
		syslog(LOG_CRIT, "%s listen socket", strerror(errno));
		exit(EXIT_FAILURE);
	}
	optval = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	lsaddr.sin_family = AF_INET;
	lsaddr.sin_port = htons((uint16_t)atoi(PORT));
	ipr = inet_pton(AF_INET, IP, &(lsaddr.sin_addr));
	if (ipr <= 0) {
		if (!ipr) {
			syslog(LOG_CRIT, "Not valid IP");
			exit(EXIT_FAILURE);
		} else {
			syslog(LOG_CRIT, "%s inet pton", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
	if (bind(ls, (sa*)&lsaddr, sizeof(lsaddr)) < 0) {
		syslog(LOG_CRIT, "%s bind", strerror(errno));
		exit(EXIT_FAILURE);
	}
	if (listen(ls, 15) < 0) {
		syslog(LOG_CRIT, "%s listen()", strerror(errno));
		return EXIT_FAILURE;
	}
	if ((flags = fcntl(ls, F_GETFL)) > -1) {
		if ((fcntl(ls, F_SETFL, flags | O_NONBLOCK)) < 0) {
			syslog(LOG_WARNING, "%s fcntl F_SETFL for O_NONBLOCK", strerror(errno));
		}
	} else {
		syslog(LOG_WARNING, "%s fcntl F_GETFL", strerror(errno));
	}
	syslog(LOG_INFO, "bbs-server started with PID %d, IP:PORT %s:%s", getpid(), IP, PORT);
	conaddr.sin_family = AF_INET;
	pollfd_ptr[pfdli].fd = ls;
	pollfd_ptr[pfdli++].events = POLLIN;
	while (loop_is_possible) /* this is the main loop */
	{
		if (pfdli == pollfd_count - 1) {
			if (extern_pollfd_array(&pollfd_ptr, &pollfd_count) < 0) {
				syslog(LOG_INFO, "Critical: realloc pollfd array");
				goto end;
			}
		}
		if (pollfd_count > POLLFD_ARR_LENGTH && pfdli < POLLFD_ARR_LENGTH / 2) {
			if (realloc_to_POLLFD_ARR_LENGTH(&pollfd_ptr, (const size_t)POLLFD_ARR_LENGTH) < 0) {
				syslog(LOG_CRIT, "Critical: realloc pollfd array to initial length");
				goto end;
			}
		}
		close_and_remove_off_connections(&first_connect, &last_connect);
		narrow_pollfd_array(pollfd_ptr, &pfdli);
		set_pollfd_by_connections(pollfd_ptr, &pfdli, (const size_t)pollfd_count,first_connect);
		ppoll_return = ppoll(pollfd_ptr, pollfd_count, NULL,  &old_mask); /* wtf? */
		if (ppoll_return < 0) {
			if (errno == EINTR) {
				if (config_update) {
					config_update = 0;
					close_and_remove_all_connections(&first_connect, &last_connect);
					free(pollfd_ptr);
					pollfd_ptr = NULL;
					free(IP);
					free(PORT);
					free(DIRECTORY);
					free(SCREEN_FILE);
					free(ACCOUNTS_FILE);
					if (SCREEN_FILE_BUF) {
						free(SCREEN_FILE_BUF);
					}
					shutdown(ls, SHUT_RDWR);
					close(ls);
					chdir("..");
					goto start;
				}
			}
		} else {
			for (pfdi = 0; pfdi < pfdli && ppoll_return; pfdi++) {
				if (pollfd_ptr[pfdi].fd != -1) {
					if (pollfd_ptr[pfdi].revents & POLLIN) {
						if (!pfdi){
							consock = accept(ls, (sa*)&conaddr, &conaddr_len);
							if (consock < 0) {
								syslog(LOG_INFO, "Unable to connect with accept");
							} else {
								if (create_connect(&first_connect, &last_connect, (const int32_t)consock) < 0) {
									syslog(LOG_INFO, "Unable to create struct connect");
								}
							}
						} else {
							recv_return = recv(pollfd_ptr[pfdi].fd, buf, sizeof(buf), 0);
							tmp = comparison_pollfd_with_connect(first_connect, (const int32_t)pollfd_ptr[pfdi].fd);
							if (!recv_return) {
								tmp->st = off;
							} else if (!strncmp((const char*)buf, "exit", 4)) {
								tmp->st = good_bye;
							} else {
								check_recv_from_tmp_and_change_state(tmp, (const char*)buf);
								memset(buf, 0, sizeof(buf));
							}
							pollfd_ptr[pfdi].fd = -1;
						}
						--ppoll_return;
					} else if (pollfd_ptr[pfdi].revents & POLLOUT) {
						tmp = comparison_pollfd_with_connect(first_connect, (const int32_t)pollfd_ptr[pfdi].fd);
						send_to_tmp_and_change_state(tmp);
						pollfd_ptr[pfdi].fd = -1;
						--ppoll_return;
					}
				}
			}
		}
	}
end:
	if (pollfd_ptr) {
		free(pollfd_ptr);
	}
	free(IP);
	free(PORT);
	free(DIRECTORY);
	free(SCREEN_FILE);
	free(ACCOUNTS_FILE);
	if (SCREEN_FILE_BUF) {
		free(SCREEN_FILE_BUF);
	}
	close_and_remove_all_connections(&first_connect, &last_connect);
	close(ls);
	syslog(LOG_INFO, "bbs-server is shutdown");
	closelog();
	return 0;
}
