#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct op {
  char* name;
  int argLength;
  char* args[10];
};

struct op handleInput(char* instr);
void handleCommand(struct op command, int *running);
int startsWith(const char *s1, const char*s2);

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

void handleCommand(struct op command, int *running) {
  if(startsWith("exit", command.name)) {
    *running = *running + 1;
  } else if(startsWith("cmdnm", command.name)) {
    printf("Command name unfinished...\n");
  } else if(startsWith("pid", command.name)) {
    printf("PID unfinished...\n");
  } else if(startsWith("systat", command.name)) {
    printf("Systat unfinished...\n");
  } else if(startsWith("help", command.name)) {
    printf("There's no helping you...\n");
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