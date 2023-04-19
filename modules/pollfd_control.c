#include "../headers/pollfd_control.h"

void fill_initial_pollfd_arr(struct pollfd *pf, const size_t pflen)
{
	size_t i;
	for (i = 0; i < pflen; i++) {
		pf[i].fd = -1;
	}
}

int32_t extern_pollfd_array(struct pollfd **pf, nfds_t *pollfd_count)
{
	size_t new_length = (size_t)pollfd_count + 500;
	size_t i;
	struct pollfd *pf_ptr;
	if (!(*pf = realloc(pf, new_length))) {
		return -1;
	}
	pf_ptr = *pf;
	for (i = (size_t)pollfd_count; i < new_length; i++) {
		pf_ptr[i].fd = -1;
	}
	*pollfd_count += 500;
	return 0;
}

int32_t realloc_to_POLLFD_ARR_LENGTH(struct pollfd **pf, size_t pfal)
{
	if (!(*pf = realloc(pf, pfal))) {
		return -1;
	}
	return 0;
}

void narrow_pollfd_array(struct pollfd *pf, size_t *pfli)
{
	size_t i;
	size_t tmp;
	size_t p = *pfli;
	for (i = 1; i < p; i++) {
		if (pf[i].fd < 0) {
			tmp = i;
			for (++i; i < p && i < 0; i++);
			if (i < p) {
				pf[tmp].fd += pf[i].fd;
				pf[i].fd = pf[tmp].fd - pf[i].fd;
				pf[tmp].fd -= pf[i].fd;
				i = tmp + 1;
			}
		}
	}
	for (i = 1; pf[i].fd > -1; i++);
	*pfli = i;
}

void set_pollfd_by_connections(struct pollfd *pfds, size_t *pfli, const size_t pfds_len, _connect *f)
{
	size_t pfi = *pfli;
	while (f) {
		if (pfi == pfds_len - 1) {
			break;
		}
		switch (f->st) {
			case screen:
			case after_screen:
			case reg_choise:
			case reg_choise_p:
			case guest_choise:
			case login_choise:
			case reg_bad_l:
			case reg_bad_p:
			case reg_success:
			case login_bad_l:
			case login_bad_p:
			case login_success:
			case unknown_command:
			case good_bye:
				pfds[pfi].fd = f->fd;
				pfds[pfi].events = POLLOUT;
				++pfi;
				break;
			case rgl_choise:
			case reg_l:
			case reg_p:
			case login_l:
			case login_p:
			case online_guest:
			case online_login:
				pfds[pfi].fd = f->fd;
				pfds[pfi].events = POLLIN;
				++pfi;
				break;
			default:
				break;
		}
		f = f->next;
		*pfli = pfi;
	}
}
