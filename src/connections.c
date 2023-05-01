#include "../headers/connections.h"

int32_t create_connect(_connect **f, _connect **l, const int32_t fd)
{
	_connect *tmp;
	int32_t fd_flags;
	if ((fd_flags = fcntl(fd, F_GETFL)) < 0) {
		syslog(LOG_INFO, "Unable to get fcntl flags: %s", strerror(errno));
		return -1;
	}
	fd_flags |= O_NONBLOCK;
	if (fcntl(fd, F_SETFL, fd_flags) < 0) {
		syslog(LOG_INFO, "Unable to set fcntl flags connect: %s", strerror(errno));
		return -1;
	}
	tmp = (_connect*)malloc(sizeof(*tmp));
	if (!tmp) {
		syslog(LOG_INFO, "Unable to create connect malloc: %s", strerror(errno));
		return -1;
	}
	tmp->fd = fd;
	tmp->st = screen;
	tmp->login = NULL;
	tmp->upload_file_name = NULL;
	memset(tmp->buf, 0, sizeof(tmp->buf));
	tmp->rights = 4;
	tmp->file_position = 0;
	tmp->next = tmp->prev = NULL;
	if (*l) {
		tmp->prev = *l;
		*l = (*l)->next = tmp;
	} else {
		*f = *l = tmp;
	}
	return 0;
}

void close_and_remove_off_connections(_connect **f, _connect **l)
{
	_connect *tmp, *tmp_free;
	tmp = *f;
	if (!tmp) {
		return;
	}
	while (tmp) {
		if (tmp->st == off) {
			if (tmp->login && tmp->login != login_guest) {
				free(tmp->login);
			}
			tmp_free = tmp;
			if (tmp->prev && tmp->next) {
				tmp->prev->next = tmp->next;
				tmp->next->prev = tmp->prev;
			} else if (tmp->prev && !tmp->next) {
				tmp->prev->next = NULL;
				*l = tmp->prev;
			} else if (!tmp->prev && tmp->next) {
				tmp->next->prev = NULL;
				*f = tmp->next;
			} else {
				*f = *l = NULL;
			}
			tmp = tmp->next;
			shutdown(tmp_free->fd, SHUT_RDWR);
			close(tmp_free->fd);
			free(tmp_free);
		} else {
			tmp = tmp->next;
		}
	}
}

void close_and_remove_all_connections(_connect **f, _connect **l)
{
	_connect *tmp, *tmp_free;
	tmp = *f;
	while (tmp) {
		shutdown(tmp->fd, SHUT_RDWR);
		close(tmp->fd);
		if (tmp->login && tmp->login != login_guest) {
			free(tmp->login);
		}
		tmp_free = tmp;
		tmp = tmp->next;
		free(tmp_free);
	}
	*f = *l = NULL;
}
