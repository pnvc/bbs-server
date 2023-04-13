#include "../headers/pollfd_control.h"

void fill_initial_pollfd_arr(struct pollfd *pf, size_t pflen)
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
	for (i = 1; i <= p; i++) {
		if (pf[i].fd < 0) {
			tmp = i;
			for (++i; i <= p && i < 0; i++);
			if (i <= p) {
				pf[tmp].fd += pf[i].fd;
				pf[i].fd = pf[tmp].fd - pf[i].fd;
				pf[tmp].fd -= pf[i].fd;
				i = tmp + 1;
			}
		}
	}
	for (i = 1; pf[i].fd > 0; i++);
	*pfli = i;
}
