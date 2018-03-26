/********************************************************************
 * signalHandle.c
 * - description?
 * NOTE: if needed
*********************************************************************/
#include "signalHandle.h"

void handle_signal(int signal) {
  sigset_t pending;
  printf("Signal %d caught!\n", signal);
}
