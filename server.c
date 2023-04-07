#include "headers/server.h"

extern volatile sig_atomic_t loop_is_possible; 	/* signal_handler.c */
extern volatile sig_atomic_t config_update;	/* signal_handler.c */

int32_t main()
{
	char *IP, *PORT, *DIRECTORY, *SCREEN_FILE, *ACCOUNTS_FILE;
	int32_t ls, ipr, optval;
	sa_in lsaddr;
	FILE *config = fopen("config/server.conf", "r");
	if (!config) {
		perror("config file\n");
		exit(EXIT_FAILURE);
	}
	if (set_config(config, &IP, &PORT, &DIRECTORY, &SCREEN_FILE, &ACCOUNTS_FILE) < 0) {
		fprintf(stderr, "set config\n");
		return 1;
	}
#if 0
	if (create_working_directory(DIRECTORY) < 0) {
		fprintf(stderr, "Unable to create working directory\n");
		return 2;
	}
	if (create_accounts_file(DIRECTORY, ACCOUNTS_FILE) < 0) {
		fprintf(stderr, "Unable to create accounts file\n");
		return 2;
	}
	if (check_image(DIRECTORY, SCREEN_FILE) < 0) {
		printf("No image\n");
	}
#endif
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
	start_deamon_process();
	openlog("bbs-server", LOG_PID, 0);
	signal(SIGUSR1, usr1_handler);
	signal(SIGHUP, hup_handler);
	syslog(LOG_INFO, "bbs-server started with PID %d, IP:PORT %s:%s", getpid(), IP, PORT);
#if 0
	while (loop_is_possible) /* this is the main loop */
	{
		;
	}
#endif
	close(ls);
	syslog(LOG_INFO, "bbs-server is shutdown");
	closelog();
	return 0;
}
