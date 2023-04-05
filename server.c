#include "headers/server.h"

int32_t main()
{
	char *IP, *PORT, *DIRECTORY, *SCREEN_FILE, *ACCOUNTS_FILE;
	int32_t ls;
	int32_t ipr;
	sa_in lsaddr;
	FILE *config = fopen("config/server.conf", "r");
	if (!config) {
		perror("config file\n");
		return EXIT_FAILURE;
	}
	if (set_config(config, &IP, &PORT, &DIRECTORY, &SCREEN_FILE, &ACCOUNTS_FILE) < 0) {
		fprintf(stderr, "set config\n");
		return EXIT_FAILURE;
	}
	ls = socket(AF_INET, SOCK_STREAM, 0);
	if (ls < 0) {
		perror("socket problem\n");
		return EXIT_FAILURE;
	}
	lsaddr.sin_family = AF_INET;
	lsaddr.sin_port = htons((uint16_t)atoi(PORT));
	ipr = inet_pton(AF_INET, IP, &(lsaddr.sin_addr));
	if (ipr <= 0) {
		if (!ipr) {
			fprintf(stderr, "Not valid IP\n");
		} else {
			perror("inet_pton problem");
		}
		return EXIT_FAILURE;
	}
	if (bind(ls, (sa*)&lsaddr, sizeof(lsaddr)) < 0) {
		perror("bind problem");
		return EXIT_FAILURE;
	}
	if (listen(ls, 15) < 0) {
		perror("listen problem");
		return EXIT_FAILURE;
	}
	/*start_deamon_process();*/
	openlog("bbs-server", LOG_PID, 0);
	/*signal(SIGUSR1, usr1_handler);
	signal(SIGHUP, hup_handler);*/
	syslog(LOG_INFO, "bbs-server started with PID %d, IP:PORT %s:%s", getpid(), IP, PORT);
	return 0;
}
