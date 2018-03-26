/********************************************************************
 * signalHandle.c
 * - A file for handling signal related events. I personally thought
 * - it was going to be bigger and not just a single function with
 * - only a print statement inside. Regardless, it paves the way for
 * - any future improvements and handling of specific signals without
 * - cluttering up the main files
*********************************************************************/
#include "signalHandle.h"

/**
 * void handle_signal(int signal)
 * - This function is where any signal given to the process lands at.
 * - Currently we just receive the signal number through a function
 * - argument and output that function to the screen.
 * Inputs: int signal - the signal number the process received
 **/
void handle_signal(int signal) {
  sigset_t pending;
  printf("Signal %d caught!\n", signal);
}
