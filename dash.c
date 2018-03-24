/********************************************************************
 * dash.c
 * 
 * Author:    Lucas Carpenter
 * Date:      02/16/2018
 * Class:     CSC456 (Operating Systems)
 * Professor: Dr. Karlsson
 * 
 * Description: DASH is a DiAgnostics SHell program. It runs a loop
 * prompting the user for a command with a 'dash>' prompt. Valid commands
 * include 'cmdnm ###' where ### is a process ID, 'pid @@@' where @@@ is 
 * a process name, 'systat', 'help', and 'exit'. cmdnm takes in a process
 * id and returns out the name of the process with the given process id,
 * pid takes in a process name and returns out all process ids with that
 * process name, systat returns out a list of system statistics including
 * cpu and memory info and OS version and uptime, help returns out a list
 * of commands, their usage, and a small description, and exit leaves the
 * diagnostic shell and returns to the bash prompt.
 *
 * Compiling:
 * $ gcc -c dash.c commands.c
 * $ gcc -o dash dash.o commands.o
 * 
 * Usage:
 * $ ./dash
 *
 ********************************************************************/

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
  /** Loop through prompt, get input, and handle command till exit is called **/
  while(running == 0) {
    printf("dash> ");
    fgets(inbuffer, 250, stdin);
    command = handleInput(inbuffer);
    if(command.name != NULL && strcmp("exit", command.name) == 0) 
      exit(0);
    
    if(command.piped > 1) {
      printf("Sorry, I can't handle more than one pipe at the moment.\n");
    }
    if(command.name != NULL) {
      cmdProc = fork();
      if(cmdProc != 0) {
	printf("Process PID: %d\n\n", cmdProc);
      } else {
	if(command.piped == 1) {
	  int p[2];
	  int pid1;
	  int pid2;
	  int i;
	  struct op cmd1;
	  struct op cmd2;
	  cmd1.name = command.name;
	  for(i = 0; i < command.pipelocs[0]; i++) {
	    cmd1.args[i] = command.args[i];
	  }
	  cmd1.args[i] = NULL;
	  cmd1.argLength = i;
	  cmd2.name = command.args[i+1];
	  int j = 0;
	  for(i = i + 2; command.args[i] != NULL; i++) {
	    cmd2.args[j++] = command.args[i];
	  }
	  cmd2.args[j] = NULL;

	  if(pipe(p) == -1) {
	    printf("Unable to pipe commands.\n");
	    continue;
	  }
      
	  pid1 = fork();
	  if(pid1 == 0) {
	    close(STDOUT_FILENO);
	    dup(p[1]); // replaces stdout with pipe write
	    close(p[0]); // closes pipe read
	    close(p[1]);
	    handleCommand(cmd1, &running);
	    exit(0);
	  }
	  pid2 = fork();
	  if(pid2 == 0) {
	    close(STDIN_FILENO);
	    dup(p[0]);//replaces stdin with pipe read
	    close(p[1]); //closes pipe write
	    close(p[0]);
	    handleCommand(cmd2, &running);
	    exit(0);
	  }
	  close(p[0]);
	  close(p[1]);
	  wait(0);
	  wait(0);
	  printf("The child process id numbers are %d and %d\n", pid1, pid2);
	} else {
	  handleCommand(command, &running);
	}
	struct rusage usage;
	getrusage(RUSAGE_SELF, &usage);
	printf("\nCPU Time:   User: %ld.%06ld sec | System: %ld.%06ld sec\n",
	       usage.ru_utime.tv_sec, usage.ru_utime.tv_usec,
	       usage.ru_stime.tv_sec, usage.ru_stime.tv_usec);
	printf("There were %lu hard page faults, %lu soft page faults, and %lu swaps.\n",
	       usage.ru_majflt, usage.ru_minflt, usage.ru_nswap);
	exit(0); // i think this goes here?
      }
      wait(0);
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
  const char delimendl[2] = "\n";
  const char delim[2] = " ";// a delimeter string indicating we split on spaces
  char* token;              // a tokenized string variable
  struct op command;        // a command structure for organizing commands
  int i = 0;                // an integer for seeing how many arguments the command has
  int curPipe = 0;
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

  if(command.name == NULL) 
    return;
  
  if(command.name[strlen(command.name)-1] == '\n') {
    command.name[strlen(command.name)-1] = '\0';
  }

  int proc1;
  int out = -1;
  int in = -1;
  int stdin_copy = dup(0);
  int stdout_copy = dup(1);
    for(int i = 0; command.args[i] != NULL; i++) {
      if(strcmp(">", command.args[i]) == 0) {
	close(1);
	out = open(command.args[i+1], O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IRGRP | S_IWGRP | S_IWUSR);
	dup2(out, 1);
	int j = i;
	while(1) {
	  if(command.args[j] == NULL)
	    break;
	  command.args[j] = command.args[j+2];
	  j++;
	}
	i--;
      } else if(strcmp("<", command.args[i]) == 0) {
	close(0);
	in = open(command.args[i+1], O_RDONLY);
	dup2(in, 0);
	int j = i;
	while(1) {
	  if(command.args[j] == NULL)
	    break;
	  command.args[j] = command.args[j+2];
	  j++;
	}
	i--;
      }
    }
  
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
    } else if(strcmp("help", command.name) == 0) {
      if(help() != 0) {
      }
    } else {
    
      int childpid = 0;
      char *args[100];
      args[0] = command.name;
      int i = 1;
    
      while(command.args[i-1] != NULL) {
	args[i] = command.args[i-1];
	i++;
      }

      args[i] = NULL;
      i = 0;
    
      childpid = fork();
      if(childpid != 0) {
      } else {
	execvp(args[0], args);
	exit(5);
      }
      int waitpid;
      int status;
      waitpid = wait(&status);
    }
    close(in);
    close(out);
    dup2(stdin_copy, 0);
    dup2(stdout_copy, 1);
    close(stdin_copy);
    close(stdout_copy);
}
