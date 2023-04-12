#ifndef POLLFD_CONTROL_H_SENTRY
#define POLLFD_CONTROL_H_SENTRY

#include <inttypes.h>
#include <stddef.h>
#include <poll.h>
#include <stdlib.h>

void fill_initial_pollfd_arr(struct pollfd *pf, size_t pflen);
int32_t extern_pollfd_array(struct pollfd **pf, nfds_t *pollfd_count);
int32_t realloc_to_POLLFD_ARR_LENGTH(struct pollfd **pf, size_t pfal);

#endif
