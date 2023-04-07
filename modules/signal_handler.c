#include "../headers/signal_handler.h"

volatile sig_atomic_t loop_is_possible = 1;
volatile sig_atomic_t config_update = 0;

void usr1_handler(int32_t s)
{
	signal(SIGUSR1, usr1_handler);
	loop_is_possible = 0;
}

void hup_handler(int32_t s)
{
	signal(SIGHUP, hup_handler);
	config_update = 1;
}
