#include "commands.h"

int systat() {
    FILE *file;
    char buffer[4096];
    size_t bytes_read;

    /** Open, read, and save to a buffer /proc/version for version information **/
    file = fopen("/proc/version", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/version is too big or nonexistent.\n");
        return 1;
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
        return 2;
    }
    /** Append a null terminator '\0' and output version info **/
    buffer[bytes_read] = '\0';
    printf("uptime:\n%s\n", buffer);

    /** Open, read, and save to a buffer /proc/uptime for uptime information **/
    file = fopen("/proc/meminfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/meminfo is too big or nonexistent.\n");
        return 3;
    }
    /** Append a null terminator '\0' and output version info **/
    buffer[bytes_read] = '\0';
    printf("meminfo:\n%s\n", buffer);
    

    /** We will need a substring for vendor_id to end of buffer
        and a substring for physical id to end of buffer **/ 
    char* starting;
    char* ending;
    /** Open, read, and save to a buffer /proc/uptime for uptime information **/
    file = fopen("/proc/cpuinfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/cpuinfo is too big or nonexistent.\n");
        return 4;
    }
    /** substring out vendor_id to end of buffer and physical id to end of buffer**/
    starting = strstr(buffer, "vendor_id");
    ending = strstr(buffer, "physical id");
    /** Subtract the two sizes and add a null terminator, then output **/
    starting[strlen(starting)-strlen(ending)] = '\0';
    printf("cpuinfo:\n%s", starting);
    return 0;
}

int pid() {
    printf("Get name of pid...\n");
    return 1;
}

int cmdnm() {
    printf("Get pid of running process...\n");
    return 1;
}

int help() {
    printf("There's no help for you....\n");
    return 1;
}