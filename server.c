#include "headers/server.h"

#define POLLFD_ARR_LENGTH 1000

extern volatile sig_atomic_t loop_is_possible, config_update;  /* signal_handler.c */

int32_t main()
{
	char *IP, *PORT, *DIRECTORY, *SCREEN_FILE, *ACCOUNTS_FILE;
	int32_t ls, ipr, optval, flags, highest_fd, consock, ppoll_return;
	sa_in lsaddr, conaddr;
  socklen_t conaddr_len = sizeof(conaddr);
  struct pollfd pollfd_arr[POLLFD_ARR_LENGTH], *pollfd_ptr = pollfd_arr;
  nfds_t pollfd_count = POLLFD_ARR_LENGTH;
  size_t pfdli = 0; /* pollfd last index */
  size_t pfdi;
  sigset_t user_mask, old_mask;
  _connect *first_connect, *last_connect;
	FILE *config = fopen("config/server.conf", "r");
	if (!config) {
		perror("config file\n");
		exit(EXIT_FAILURE);
	}
	if (set_config(config, &IP, &PORT, &DIRECTORY, &SCREEN_FILE, &ACCOUNTS_FILE) < 0) {
		fprintf(stderr, "set config\n");
		return 1;
	}
  fclose(config);
	if (create_working_directory_and_chdir(DIRECTORY) < 0) {
		fprintf(stderr, "Unable to create working directory\n");
		return 2;
	}
	if (create_accounts_file((const char*)ACCOUNTS_FILE) < 0) {
		fprintf(stderr, "Unable to create accounts file\n");
		return 2;
	}
	if (check_image((const char*)SCREEN_FILE) < 0) {
		printf("No image\n");
	}
  fill_initial_pollfd_arr(pollfd_ptr, POLLFD_ARR_LENGTH);
  first_connect = last_connect = NULL;
	ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls < 0) {
		perror("socket problem\n");
		exit(EXIT_FAILURE);
	}
	optval = 1;
	setsockopt(ls, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
	lsaddr.sin_family = AF_INET;
	lsaddr.sin_port = htons((uint16_t)atoi(PORT));
	ipr = inet_pton(AF_INET, IP, &(lsaddr.sin_addr));
	if (ipr <= 0) {
		if (!ipr) {
			fprintf(stderr, "Not valid IP\n");
			return 3;
		} else {
			perror("inet_pton");
			exit(EXIT_FAILURE);
		}
	}
	if (bind(ls, (sa*)&lsaddr, sizeof(lsaddr)) < 0) {
		perror("bind problem");
		return EXIT_FAILURE;
	}
	if (listen(ls, 15) < 0) {
		perror("listen problem");
		return EXIT_FAILURE;
	}
  flags = fcntl(ls, F_GETFL);
  fcntl(ls, F_SETFL, flags | O_NONBLOCK);
	start_deamon_process();
	openlog("bbs-server", LOG_PID, 0);
	signal(SIGUSR1, usr1_handler);
	signal(SIGHUP, hup_handler);
  sigemptyset(&user_mask);
  sigaddset(&user_mask, SIGUSR1);
  sigaddset(&user_mask, SIGHUP);
	syslog(LOG_INFO, "bbs-server started with PID %d, IP:PORT %s:%s", getpid(), IP, PORT);
  conaddr.sin_family = AF_INET;
  pollfd_ptr[pfdli].fd = ls;
  pollfd_ptr[pfdli++].events = POLLIN;
  sigprocmask(SIG_SETMASK, &user_mask, &old_mask);
	while (loop_is_possible) /* this is the main loop */
	{ 
    ppoll_return = ppoll(pollfd_ptr, pollfd_count, NULL,  &old_mask); /* wtf? */
    if (ppoll_return < 0) {
      if (errno == EINTR) {
        if (config_update) {
          syslog(LOG_INFO, "AHAHA");
          config_update = 0;
        }
      }
    } else {
      for (pfdi = 0; pfdi <= pfdli && ppoll_return; pfdi++) {
        if (pollfd_ptr[pfdi].fd != -1) {
          --ppoll_return;
          if (!pfdi && pollfd_ptr[pfdi].revents & POLLIN) {/* ACCEPT CONNECTION REQUEST */
            consock = accept(ls, (sa*)&conaddr, &conaddr_len);
            if (consock < 0) {
              syslog(LOG_INFO, "Unnable to connect with accept");
            } else {
              if (create_connect(&first_connect, &last_connect, (const int32_t)consock) < 0) {
                syslog(LOG_INFO, "Unable to create struct connect");
              }
            }
          }                                                                    /* ACCEPT CONNECTION REQUEST */
          else if (pollfd_ptr[pfdi].revents & POLLIN) {
            ;
          }
          else if (pollfd_ptr[pfdi].revents & POLLOUT) {
            ;
          }
        }
      }
    }
  }
	close(ls);
	syslog(LOG_INFO, "bbs-server is shutdown");
	closelog();
	return 0;
}
