#ifndef SIGNAL_HANDLER_H_SENTRY
#define SIGNAL_HANDLER_H_SENTRY

#include <inttypes.h>
#include <signal.h>

void usr1_handler(int32_t s);
void hup_handler(int32_t s);

#endif
