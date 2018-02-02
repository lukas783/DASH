#include <stdio.h>
#include <string.h>

void handleInput(char* instr);

int main(int argc, char*argv[]) {
  char inbuffer[256] = "";
  int running = 0;
  while(running == 0) {
    printf("dash> ");
    scanf("%s", &inbuffer);
    if(strcmp("exit",inbuffer) == 0) {
      running++;
    }
    /** PRETTY UP THE STRING, SEND TO HANDLE FUNCTION **/
  }
  return 0;
}

void handleInput(char* instr) {

}