#include "../headers/pollfd_control.h"

void fill_initial_pollfd_arr(struct pollfd *pf, size_t pflen)
{
  size_t i;
  for (i = 0; i < pflen; i++) {
    pf[i].fd = -1;
  }
}
