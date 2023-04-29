#define _XOPEN_SOURCE 500
#include "../headers/client.h"

char buf[1450];

int32_t main(int32_t argc, const char **argv)
{
	int32_t ms, ipr, recv_ret, ms_flags, poll_return, loop_is_possible = 1;
	sa_in cs_addr;
	struct pollfd pptr;
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
	pptr.events = POLLIN | POLLOUT;
	while (loop_is_possible) {
		usleep(10000);
		poll_return = poll(&pptr, 1, 0);
		if (poll_return < 0) {
			perror("Poll");
			exit(EXIT_FAILURE);
		} else {
			if (pptr.revents & POLLIN) {
				recv_ret = recv(ms, buf, sizeof(buf), 0);
				write(1, (const char*)buf, recv_ret);
			} else if (pptr.revents & POLLOUT) {
				read(0, buf, sizeof(buf));
				send(ms, (const char*)buf, strlen(buf), 0);
			}
			pptr.revents = 0;
		}
		memset(buf, 0, strlen(buf));
	}
	close(ms);
	return 0;
}
