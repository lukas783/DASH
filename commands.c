#include "commands.h"

void systat() {
    printf("Systat unfinished.......\n");
    FILE *file;
    char buffer[1024];
    size_t bytes_read;
    char* match;
    float clock_speed;

    /** Open, read, and save to a buffer /proc/version for version information **/
    file = fopen("/proc/version", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/version is too big or nonexistent.\n");
        return;
    }
    /** Append a null terminator '\0' and output version info **/
    buffer[bytes_read] = '\0';
    printf("\nversion:\n%s\n", buffer);


    /** Open, read, and save to a buffer /proc/uptime for uptime information **/
    file = fopen("/proc/uptime", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/uptime is too big or nonexistent.\n");
        return;
    }
    /** Append a null terminator '\0' and output version info **/
    buffer[bytes_read] = '\0';
    printf("\nuptime:\n%s\n", buffer);

    

/*
    match = strstr(buffer, "cpu MHz");
    if(match == NULL) {
        printf("/proc/cpuinfo is missing string 'cpu Mhz'\n");
        return;
    }
    sscanf(match, "cpu MHz :  %f", &clock_speed);
    printf("Clock speed is... %f\n", clock_speed);
*/
}