#ifndef POLLFD_CONTROL_H_SENTRY
#define POLLFD_CONTROL_H_SENTRY

#include <inttypes.h>
#include <stddef.h>
#include <poll.h>

void fill_initial_pollfd_arr(struct pollfd *pf, size_t pflen);

#endif
