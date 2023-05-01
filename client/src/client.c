#define _XOPEN_SOURCE 500
#include "../headers/client.h"

char buf[1450];

int32_t main(int32_t argc, const char **argv)
{
	int32_t ms, ipr, recv_return, ms_flags, poll_return, loop_is_possible;
	uint64_t totaly_uploaded = 0;
	char uploading, downloading, updo_f_is_opened;
	sa_in cs_addr;
	struct pollfd pptr;
	FILE *upload_f, *download_f;
	size_t fread_return;
	uploading = downloading = 1;
	updo_f_is_opened = 0;
	loop_is_possible = 1;
	if (argc < 3) {
		printf("You need args: IP PORT\n");
		return 1;
	}
	if ((ms = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Main socket");
		exit(EXIT_FAILURE);
	}
	cs_addr.sin_family = AF_INET;
	cs_addr.sin_port = htons((uint16_t)(atoi(argv[2])));
	ipr = inet_pton(AF_INET, argv[1], &(cs_addr.sin_addr));
	if (ipr <= 0) {
		if (!ipr) {
			fprintf(stderr, "Not valid server IP");
			exit(EXIT_FAILURE);
		} else {
			perror("Inet pton");
			exit(EXIT_FAILURE);
		}
	}
	if (connect(ms, (const sa*)&cs_addr, sizeof(cs_addr)) < 0) {
		perror("Connect");
		exit(EXIT_FAILURE);
	}
	if ((ms_flags = fcntl(ms, F_GETFL)) > -1) {
		if (fcntl(ms, F_SETFL, ms_flags | O_NONBLOCK) < 0) {
			perror("Fcntl set O_NONBLOCK flag");
			exit(EXIT_FAILURE);
		}
	} else {
		perror("fcntl get flags");
		exit(EXIT_FAILURE);
	}
	pptr.fd = ms;
	pptr.events = 0;
	pptr.events = POLLIN;
	sleep(1);
	while (loop_is_possible) {
		usleep(10000);
		poll_return = poll(&pptr, 1, 0);
		if (poll_return < 0) {
			perror("Poll");
			exit(EXIT_FAILURE);
		} else {
			if (pptr.revents & POLLIN) {
				recv_return = recv(ms, buf, sizeof(buf), 0);
				if (!recv_return) {
					break;
				}
				if (!(pptr.events & POLLOUT)) {
					if (!strncmp((const char*)buf, "#", 1)) {
						pptr.events |= POLLOUT;
					}
				}
				write(1, (const char*)buf, recv_return);
				if (downloading) {
					if (!(uploading = strncmp((const char*)buf, "> Upload:", 9))) {
						pptr.events &= ~POLLIN;
					}
				}
				if (uploading) {
					;
				}
			} else if (pptr.revents & POLLOUT) {
				if (!uploading) {
					if (!updo_f_is_opened) {
						read(0, buf, sizeof(buf));
						updo_f_is_opened = 1;
						*(strchr((const char*)buf, '\n')) = 0;
						upload_f = fopen((const char*)buf, "r");
						if (!upload_f) {
							perror("Upload file opening");
							exit(EXIT_FAILURE);
						}
						updo_f_is_opened = 1;
						memset(buf, 0, strlen((const char*)buf));
					}
					fread_return = fread(buf, sizeof(char), sizeof(buf), upload_f);
					if (!fread_return) {
						if (feof(upload_f)) {
							uploading = 1;
							updo_f_is_opened = 0;
							fclose(upload_f);
							pptr.events |= POLLIN;
							memset(buf, 0, 1450);
							printf("Totaly uploaded %lu bytes\n", totaly_uploaded);
							totaly_uploaded = 0;
							usleep(100000);
						} else {
							perror("Read from upload file");
							exit(EXIT_FAILURE);
						}
					} else {
						send(ms, (const char*)buf, fread_return, 0);
						totaly_uploaded += fread_return;
					}
				} else {
					read(0, buf, sizeof(buf));
					send(ms, (const char*)buf, strlen(buf), 0);
				}
			}
			pptr.revents = 0;
		}
		memset(buf, 0, strlen(buf));
	}
	close(ms);
	return 0;
}
