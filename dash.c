/********************************************************************
 * dash.c
 * 
 * Author:    Lucas Carpenter
 * Date:      03/26/2018
 * Class:     CSC456 (Operating Systems)
 * Professor: Dr. Karlsson
 * 
 * Description: DASH is a DiAgnostics SHell program. It runs a loop
 * prompting the user for a command with a 'dash>' prompt. A list of valid
 * commands and their usage can be pulled up using the 'help' command. The 
 * program handles running a set of intrinsic commands like 'pid','cmdnm',
 * 'systat', 'signal', and 'cd' while giving access to linux commands from
 * the /bin/ directory. The shell can also handle redirecting stdout and/or
 * stdin as well as piping one commands output as another commands input. 
 * The program achieves this by running each command as a forked process and
 * then handling each command, if the command is an intrinsic function of the
 * shell then no other process is forked, if the command is not an intrinsic 
 * function, another process is forked and the command is executed on that 
 * process instead, returning when finished. Any signal received by the program
 * will be caught by the diagnostic shell as well as the signal number will
 * be displayed when received.
 * 
 * Compiling:
 * $ gcc -c dash.c commands.c signalHandle.c
 * $ gcc -o dash dash.o commands.o signalHandle.o
 * 
 * Usage:
 * $ ./dash
 * 
 * Bugs: 
 * 
 * NOTE: The piping, redirects, signal handling, and any subfunctions 
 *       pertaining to them contain modified code used from "Advanced 
 *       Linux Programming" by CodeSourcery LLC and "The Open Group Base 
 *       Specifications" by IEEE and The Open Group; 
 *       See their COPYRIGHT for more information
 *
 ********************************************************************/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include "commands.h"
#include "signalHandle.h"

/**
 * struct op
 * - A structure for an input command that stores
 * - the current command name, up to 10 arguments,
 * - and how many arguments are in the current command.
**/
struct op {
  char* name;
  int argLength;
  char* args[10];
  int piped;
  int pipelocs[10];
};

/** Function Prototypes for dash.c **/
struct op handleInput(char* instr);
void handleCommand(struct op command, int *running);
int startsWith(const char *s1, const char*s2);

/**
 * int main(int argc, char*argv[])
 * - The program entry point. We declare an input buffer, a running boolean,
 * - and a command structure, we then proceed to loop until running is modified
 * - by prompting the user with 'dash> ' then getting input and processing it
 * - if the command was exit, running is modified and the function exits with
 * - a return code of 0
**/
int main(int argc, char*argv[]) {
  char inbuffer[256] = ""; // A buffer for reading in input line 
  int running = 0;         // A boolean for finding if exit has been called
  struct op command;       // a command structure for handling commands
  int cmdProc;

  /** Set up a signal handler and pass signals 0 to 29 to it**/
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = handle_signal;
  for(int i = 0; i < 30; i++) {
    sigaction(i, &sa, 0);
  }

  /** Loop through prompt, get input, and handle command till exit is called **/
  while(running == 0) {
    printf("dash> ");
    fgets(inbuffer, 250, stdin);
    command = handleInput(inbuffer);
    
    /** If we have more than 1 pipe in our input, we will let them know we cant handle it **/
    if(command.piped > 1) {
      printf("Sorry, I can't handle more than one pipe at the moment.\n");
    }

    /** If we don't have a null input and we aren't exiting the program or doing cd, we handle command **/
    if(command.name != NULL &&
      strcmp("exit", command.name) != 0 &&
      strcmp("cd", command.name) != 0) {

      /** Fork the first process print the child pid and handle whether it's piped or not **/
      cmdProc = fork();
      if(cmdProc != 0) {
	      printf("Process PID: %d\n\n", cmdProc);
      } else {
	      if(command.piped == 1) {
          /** Create variables to hold the pipe fds and child pids **/
          int p[2];
          int pid1;
          int pid2;
          int i;
          /** Create two new command structs to split commands into **/
          struct op cmd1;
          struct op cmd2;
          /** Tokenize the left side of the pipe into a valid command struct **/
          cmd1.name = command.name;
          for(i = 0; i < command.pipelocs[0]; i++) {
            cmd1.args[i] = command.args[i];
          }
          cmd1.args[i] = NULL;
          cmd1.argLength = i;
          /** Tokenize the right side of the pipe into a valid command struct **/
          cmd2.name = command.args[i+1];
          int j = 0;
          for(i = i + 2; command.args[i] != NULL; i++) {
            cmd2.args[j++] = command.args[i];
          }
          cmd2.args[j] = NULL;

          /** Check for a valid pipe **/
          if(pipe(p) == -1) {
            printf("Unable to pipe commands.\n");
            continue;
          }
            
          /** Fork the left side of the pipe and handle it **/
          pid1 = fork();
          if(pid1 == 0) {
            /** Close off stdout, replace stdout with our pipe, handle, and leave **/
            close(STDOUT_FILENO); // closes the std output stream
            dup(p[1]); // replaces the lowest fd with p[1] write pipe
            close(p[0]); // close our pipes read end
            close(p[1]); // close our pipes write end
            handleCommand(cmd1, &running); // handle the command
            exit(0);
          }
          /** Fork the right side of the pipe and handle it **/
          pid2 = fork();
          if(pid2 == 0) {
            close(STDIN_FILENO); // close the std input stream
            dup(p[0]);   // replaces the lowest fd with p[0] read pipe
            close(p[1]); // close our pipes write end
            close(p[0]); // close our pipes read end
            handleCommand(cmd2, &running); // handle the command
            exit(0);
          }
          /** Close everything up and wait for the children to exit **/
          close(p[0]);
          close(p[1]);
          wait(0);
          wait(0);
          printf("The child process id numbers are %d and %d\n", pid1, pid2);
        } else { 
          /** If we aren't piping we just handle the command normally **/
          handleCommand(command, &running);
        }
        /** Get process statistics and output them to stdout **/
        struct rusage usage;
        getrusage(RUSAGE_SELF, &usage);
        printf("\nCPU Time:   User: %ld.%06ld sec | System: %ld.%06ld sec\n",
              usage.ru_utime.tv_sec, usage.ru_utime.tv_usec,
              usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
        printf("There were %lu hard page faults, %lu soft page faults, and %lu swaps.\n",
              usage.ru_majflt, usage.ru_minflt, usage.ru_nswap);
        exit(0);
      }
      /** Wait for the children to finish so we can continue looping**/
      wait(0);
    } else {
      /** If we aren't forking, just handle the command normally in the same process**/
      handleCommand(command, &running);
    }
  }
  return 0;
}

/**
 * op handleInput(char* instr)
 * - handleInput cycles through the given input string, tokenizes
 * - with strtok() on space characters and produces a command structure
 * - with the given line, the first token is the command name and each
 * - subsequent token is a command argument (up to 10 arguments). Once
 * - processed, the structure is returned out and the command is parsed.
 * Inputs:  char* instr - the input string to tokenize into a command
 * Outputs: op command - a command structure containing a parsed command.
**/
struct op handleInput(char* instr) {
  const char delimendl[2] = "\n"; // a delimeter string indicating we split on new line
  const char delim[2] = " ";// a delimeter string indicating we split on spaces
  char* token;              // a tokenized string variable
  struct op command;        // a command structure for organizing commands
  int i = 0;                // an integer for seeing how many arguments the command has
  int curPipe = 0;          // a variable to hold how many pipe characters we have
  /** Remove stray newlines.. **/
  instr = strtok(instr, delimendl);
  
  /** Tokenize our first item and declare the command name **/
  token = strtok(instr, delim);
  command.name = token;
  /** Loop through each subsequent token (up to 10) adding as a command argument **/
  for(i = 0; i < 10; i++) {
    command.args[i] = strtok(NULL, delim);
    if(command.args[i] != NULL && strcmp("|", command.args[i]) == 0) {
      command.pipelocs[curPipe] = i;
      curPipe++;
    }
  }

  /** Tell our command how many pipes exist in the command **/
  command.piped = curPipe;
  /** Add the length of the input command arguments to our structure and return it**/
  command.argLength = i;
  return command;
}

/** 
 * void handleCommand(struct op command, int* running)
 * - handleCommand takes in a command structure and the running boolean.
 * - The function begins by comparing the given command name with various
 * - valid commands. If the command name given matches 'exit', running is
 * - modified and the program will exit, otherwise the function attempts
 * - to pass the command and the valid arguments to their helper functions.
 * - If no valid helper function exists, an error message is displayed
 * - prompting the user to type 'help' for a list of valid commands
 * Inputs: op command - a command structure to identify a given command
 *         int* running - a pointer to an integer to modify the running boolean
 * Outputs: None
**/
void handleCommand(struct op command, int *running) {

  /** If we have no command to handle, get outta here before we break **/
  if(command.name == NULL) 
    return;
  
  /** If a stray newline got through, replace with a null terminator **/
  if(command.name[strlen(command.name)-1] == '\n') {
    command.name[strlen(command.name)-1] = '\0';
  }

  /** Set up to handle redirecting  **/
  int proc1;
  int out = -1;
  int in = -1;
  int stdin_copy = dup(0);
  int stdout_copy = dup(1);
  /** Loop through each command argument looking for a '<' or a '>' **/
  for(int i = 0; command.args[i] != NULL; i++) {
    /** Handle redirecting output by opening a new file stream and replacing our stdout's fd with it**/
    if(strcmp(">", command.args[i]) == 0) {
	    close(1);
	    out = open(command.args[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	    dup2(out, 1);
      int j = i;
      /** We must remove the redirect arguments **/
      while(1) {
        if(command.args[j] == NULL)
          break;
        command.args[j] = command.args[j+2];
        j++;
      }
      i--;
    /** Handle redirecting input by closing the stdin fd and replaing with a diff fd**/
    } else if(strcmp("<", command.args[i]) == 0) {
      close(0);
      in = open(command.args[i+1], O_RDONLY);
      dup2(in, 0);
      int j = i;
      /** We must remove the redirect arguments **/
      while(1) {
        if(command.args[j] == NULL)
          break;
        command.args[j] = command.args[j+2];
        j++;
      }
      i--;
    }
  }
  
  /** Handle 'intrinsic' functions **/
  if(strcmp("exit", command.name) == 0) {
    *running = *running + 1;
  } else if(strcmp("cmdnm", command.name) == 0) {
    if(cmdnm(command.args[0]) != 0) {
    }
  } else if(strcmp("pid", command.name) == 0) {
    if(pid(command.args[0]) != 0) {
    }
  } else if(strcmp("systat", command.name) == 0) {
    if(systat() != 0) {
    }
  } else if(strcmp("cd", command.name) == 0) {
    if(cd(command.args[0]) != 0) {
    }
  } else if(strcmp("signal", command.name) == 0) {
    if(sig(command.args) != 0) {
    }
  } else if(strcmp("help", command.name) == 0) {
    if(help() != 0) {
    }
  /** Handle linux commands **/
  } else {
    /** Create variables for repositioning arguments and child pids **/
    int childpid = 0;
    char *args[100];

    /** Build a new list of arguments from our command structure **/
    args[0] = command.name;
    int i = 1;
    while(command.args[i-1] != NULL) {
	    args[i] = command.args[i-1];
	    i++;
    }

    /** A sanity check, replacing the last argument with NULL just in case **/
    args[i] = NULL;
    i = 0;
    
    /** Fork our process and execute the linux command, if not valid printf will let you know. **/
    childpid = fork();
    if(childpid != 0) {
    } else {
	    execvp(args[0], args);
	    printf("You've entered an invalid UNIX command.\n");
	    exit(5);
    }
    /** Wait for our child process to finish up **/
    int waitpid;
    int status;
    waitpid = wait(&status);
  }
  /** replace our redirected output with the stdin and stdout copies we made earlier **/
  close(in);
  close(out);
  dup2(stdin_copy, 0);
  dup2(stdout_copy, 1);
  close(stdin_copy);
  close(stdout_copy);
}
