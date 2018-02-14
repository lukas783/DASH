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
  char inbuffer[256] = "";
  int running = 0;
  struct op command;
  while(running == 0) {
    printf("dash> ");
    fgets(inbuffer, 250, stdin);
    command = handleInput(inbuffer);
    handleCommand(command, &running);
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
  char* tokens[10];
  const char delim[2] = " ";
  char* token;
  struct op command;
  token = strtok(instr, delim);
  command.name = token;
  int i = 0;
  for(i = 0; i < 10; i++) {
    command.args[i] = strtok(NULL, delim);
    if(command.args[i] == NULL)
      break;
  }

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
  //TODO : REMOVE STARTS WITH, REPLACE WITH A PROPER COMPARE
  if(startsWith("exit", command.name)) {
    *running = *running + 1;
  } else if(startsWith("cmdnm", command.name)) {
    if(cmdnm(command.args[0]) != 0) {
      printf("There was an error getting process information. Try again later, or restart the program.\n");
    }
  } else if(startsWith("pid", command.name)) {
    if(pid(command.args[0]) != 0) {
      printf("There was an error getting process information. Try again later, or restart the program.\n");
    }
  } else if(startsWith("systat", command.name)) {
    if(systat() != 0) {
      printf("There was an error getting system information. Try again later, or restart the program.\n");
    }
  } else if(startsWith("help", command.name)) {
    if(help() != 0) {
      printf("There was an error getting help information. Try again later, or restart the program.\n");
    }
  } else {
    printf("Unknown command name. For a list of commands and their usage, type help.\n");
  }
}

/** 
 * int startsWith(const char*, const char*)
 * - A helper function that compares if string 2 matches string 1
 * - up to strlen(s1) characters. This is useful for when scanf picks
 * - up the newline character and the same command becomes two different
 * - commands in the eyes of strcmp.
 * Arguments: const char* s1 - a prefix string
 *            const char* s2 - a string to compare to s1
 * Returns - 1 if s2 starts with s1, and 0 otherwise
**/
int startsWith(const char *s1, const char*s2) {
  return strncmp(s1, s2, strlen(s1)) == 0;
}
