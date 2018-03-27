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
  switch(signal) {
    case 1:
      printf("SIGHUP caught, hangup detected on controlling terminal or death of controlling process.\n");
    break;
    case 2:
      printf("SIGINT caught, CTRL+C detected, an interrupt from keyboard.\n");
      exit(5);
    break;
    case 3:
      printf("SIGQUIT caught, Quit from keyboard.\n");
      exit(5);
    break;
    case 4:
      printf("SIGILL caught, illegal instruction.\n");
    break;
    case 6:
      printf("SIGABRT caught, abort signal from abort function.\n");
    break;
    case 7:
    case 10:
      printf("SIGBUS/SIGPOLL caught, buss error  or a pollable event detected.\n");
    break;
    case 8:
      printf("SIGFPE caught, floating point exception.\n");
    break;
    case 9:
      printf("SIGKILL caught, kill signal, can't catch!.\n");
    break;
    case 11:
      printf("SIGSEGV caught, invalid memory reference (segmentation fault).\n");
      exit(5);
    break;
    case 12:
    case 31:
      printf("SIGSYS caught, bad system call.\n");
    break;
    case 13:
      printf("SIGPIPE caught, broken pipe discovered, patch the leak!\n");
    break;
    case 14:
      printf("SIGALRM caught, I bet you thought I'd make that annoying sound again..\n");
    break;
    case 15:
      printf("SIGTERM caught, termination signal, its a polite kill signal, still not catchable.\n");
    break;
    case 16:
    case 30:
      printf("SIGUSR1 caught, you should ask whoever sent that signal what they were trying to accomplish.");
    break;
    case 17:
    case 18:
    case 20:
      printf("SIGCHLD caught, a child process has been killed.\n");
    break;
    
    default:
      printf("Signal %d caught! \n", signal);
      break;
  }
}
