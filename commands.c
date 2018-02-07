#include "commands.h"

void systat() {
    printf("Systat unfinished.......\n");
    File *file;
    char buffer[1024];
    size_t bytes_read;
    char* match;
    float clock_speed;

    file = fopen("/proc/cpuinfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
        //panic, explode, self destruct, leave no witnesses
    }

}