#define _XOPEN_SOURCE 500
#include "../headers/client.h"

char buf[1450];
char buf_nfn[50];
static const char download_choose_your_file[] = "Choose the file name for the downloading file:\n";
static const char file_already_exists[] = "File with this name has already exists, choose another name\n";

int32_t main(int32_t argc, const char **argv)
{
	int32_t ms, ipr, recv_return, ms_flags, poll_return, loop_is_possible, fd_df;
	uint64_t totaly_uploaded = 0, totaly_downloaded = 0;
	char uploading, downloading, updo_f_is_opened, list_command, file_name_is_possible, *download_end_check_cmds;
	sa_in cs_addr;
	struct pollfd pptr;
	FILE *upload_f;
	size_t fread_return;
	uploading = downloading = 1;
	file_name_is_possible = updo_f_is_opened = 0;
	loop_is_possible = 1;
	list_command = 0;
	fd_df = 0;
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
					loop_is_possible = 0;
				}
				if (!downloading) {
					if (!updo_f_is_opened) {
						if (!strncmp((const char*)buf, "> Unable download\n", 18)) {
							if (!buf[18]) {
								downloading = 1;
								goto skip_downloading_error;
							} else if (buf[18] == '>') {
								downloading = 1;
								write(1, (const char*)buf, recv_return);
								pptr.events |= POLLOUT;
								pptr.events &= ~POLLIN;
								goto next;
							}
						}
						while (!file_name_is_possible) {
							write(1, download_choose_your_file, sizeof(download_choose_your_file) - 1);
							read(0, buf_nfn, sizeof(buf_nfn));
							*strchr((const char*)buf_nfn, '\n') = 0;
							if ((fd_df = open((const char*)buf_nfn, O_WRONLY | O_APPEND | O_CREAT | O_EXCL, 0664))) {
								updo_f_is_opened = file_name_is_possible = 1;
							} else if (errno == EEXIST) {
								write(1, file_already_exists, sizeof(file_already_exists) - 1);
							} else {
								printf("kapec\n");
								loop_is_possible = 0;
								goto next;
							}
							memset(buf_nfn, 0, sizeof(buf_nfn));
						}
					}
					if (recv_return < 1450) {
						downloading = 1;
						updo_f_is_opened = file_name_is_possible = 0;
						download_end_check_cmds = strstr((const char*)buf, "> You have");
						if (download_end_check_cmds) {
							if (!(buf - download_choose_your_file)) {
								write(1, (const char*)buf, recv_return);
								pptr.events |= POLLOUT;
								pptr.events &= ~POLLIN;
								goto end_download;
							}
							*download_end_check_cmds = 0;
						}
						totaly_downloaded += strlen((const char*)buf);
						write(fd_df, (const char*)buf, strlen((const char*)buf));
						if (download_end_check_cmds) {
							*download_end_check_cmds = '>';
							write(1, (const char*)download_end_check_cmds, strlen((const char*)download_end_check_cmds));
							download_end_check_cmds = NULL;
							pptr.events |= POLLOUT;
							pptr.events &= ~POLLIN;
						}
end_download:			close(fd_df);
						printf("Download success. Totaly uploaded %lu bytes\n", totaly_downloaded);
						totaly_downloaded ^= totaly_downloaded;
					} else if (recv_return == 1450) {
						totaly_downloaded += 1450;
						write(fd_df, (const char*)buf, 1450);
					}
					goto next;
				}
skip_downloading_error:
				write(1, (const char*)buf, recv_return);
				if (strchr(buf, '>')) {
					pptr.events &= ~POLLIN;
					pptr.events |= POLLOUT;
					if (list_command) {
						list_command = 0;
					}
				}
				if (downloading && !list_command) {
					if (!strncmp((const char*)buf, "> Upload:", 9)) {
						uploading = 0;
					} else if (!strncmp((const char*)buf, "> Download:", 11)) {
						downloading = 0;
					}
				}
				if (uploading) {
				}
			} else if (pptr.revents & POLLOUT) {
				if (!uploading) {
					if (!updo_f_is_opened) {
						read(0, buf, sizeof(buf));
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
							if (!totaly_uploaded) {
								send(ms, "00000-----", 10, 0);
							}
							pptr.events |= POLLIN;
							pptr.events &= ~POLLOUT;
							printf("Upoload success. Totaly uploaded %lu bytes\n", totaly_uploaded);
							totaly_uploaded ^= totaly_uploaded;
						} else {
							perror("Read from upload file");
							exit(EXIT_FAILURE);
						}
					} else {
						send(ms, (const char*)buf, fread_return, 0);
						totaly_uploaded += fread_return;
					}
				} else {
					if (list_command) {
						pptr.events &= ~POLLOUT;
					} else {
						read(0, buf, sizeof(buf));
						if (!strncmp((const char*)buf, "LIST\n", 5) && !buf[5]) {
							list_command = 1;
							if (!downloading) {
								downloading = 1;
							}
						}
						send(ms, (const char*)buf, strlen(buf), 0);
						pptr.events &= ~POLLOUT;
						pptr.events |= POLLIN;
					}
				}
			}
next:		pptr.revents = 0;
		}
		memset(buf, 0, strlen(buf));
	}
	close(ms);
	return 0;
}
