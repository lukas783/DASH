#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct op {
  char* name;
  char* args[10];
};

struct op handleInput(char* instr);


int main(int argc, char*argv[]) {
  char inbuffer[256] = "";
  int running = 0;
  struct op command;
  while(running == 0) {
    printf("dash> ");
    fgets(inbuffer, 250, stdin);
    if(strcmp("exit",inbuffer) == 0) {
      running++;
    }
    command = handleInput(inbuffer);
    if(strcmp(command.name, "exit"))
      running++;
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
  return command;
}