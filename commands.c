/********************************************************************
 * commands.c
 * - A file of helper functions to handle given commands
*********************************************************************/
#include "commands.h"

/**
 * int systat()
 * - systat attempts to open and output information from four files
 * - in the linux system. All four files are located in /proc/ and
 * - include the entire version file, the entire uptime file, the entire
 * - meminfo fie and a portion of the cpuinfo file. Each file opens
 * - their respective file with a FILE pointer, reads it to a buffer of
 * - 4,096 bytes and closes the file immediately, if the file was unreadable
 * - or empty an error message is displayed, otherwise a null terminator is 
 * - appended and the contents are output to stdout. In the case of cpuinfo,
 * - strstr is used to get the contents from vendor_id on down, and then the
 * - contents from physical_id on down, the difference in size is found and
 * - a null terminator is put at that position in the vendor_id substring to
 * - output only information between the two areas of text.
 * Inputs: None
 * Ouptuts: 0 on Success
 *          1 on failed to open /proc/version
 *          2 on failed to open /proc/uptime
 *          3 on failed to open /proc/meminfo
 *          4 on failed to open /proc/cpuinfo
**/
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

/**
 * int cmdnm(char* pid)
 * - cmdnm takes in a process id as a string, the process id is appended
 * - to a '/proc/' prefix and a '/comm' postfix is attached as well. This
 * - new string should lead to a process name as long as the given process
 * - id is valid, if the file can't be opened, the process isn't valid and
 * - an error message is displayed and returned out. If the process exists,
 * - the contents of that process ID's comm folder are read in through a buffer
 * - and displayed out.
 * Inputs: char* pid - a process ID given for searching for a matching process name
 * Outputs: stdout: If the given pid is valid, a process name, otherwise an 
 *                  error message
 *          0 if Succesful
 *          1 if unable to open /proc/[char* pid]/comm
**/
int cmdnm(char* pid) {
    FILE *file;
    size_t bytes_read = 0;
    char buffer[512]; // i really hope there aren't any program names with a process name longer than 511 chars!
    char filePath[512] = "/proc/";
    if(pid[strlen(pid)-1] == '\n') {
      pid[strlen(pid)-1] = '\0';
    }
    strncat(filePath, pid, strlen(pid));
    strncat(filePath, "/comm", 5);
    file = fopen(filePath, "r");
    if(file != NULL) {
      bytes_read = fread(buffer, 1, sizeof(buffer), file);
      fclose(file);
    }
    if(bytes_read == 0 || bytes_read == sizeof(buffer)) {
      printf("Process ID for '%s' does not exist or can not be read.\n", pid);
      return 1;
    }
    buffer[bytes_read] = '\0';
    printf("%s", buffer);
    return 0;
}

/**
 * int pid(char* cmdnm)
 * - pid takes in a process name as a string, traverses to the /proc/ folder
 * - and attempts to open it. If the /proc/ folder can be opened, the current
 * - directories include all active processes and each folder with a number
 * - is considered a valid process. The function then tries to open that folder's
 * - comm file and match the input string to the contents of the comm file.
 * - If the process name matches the given string that process ID is printed out
 * - otherwise the file is closed. The function searches through the entire directory
 * - until there are no more processes to check, then closes the directory and exits.
 * Inputs: char* cmdnm - A process name to match to active pids
 * Outputs: stdout: any match of process name to pid outputs that pid
 *          0 if successful
 *          1 if the /proc/ directory could not be opened
**/
int pid(char* cmdnm) {
  //    printf("Get all pids of given process...\n");
    char procRoot[7] = "/proc/";
    DIR *dir;
    long pid;
    char *next;
    struct dirent *dirInfo;
    FILE *file;
    char buffer[512];
    size_t bytes_read;

    if(dir=opendir(procRoot)) {
        while((dirInfo = readdir(dir)) != NULL) {
            pid=strtol(dirInfo->d_name, &next, 10);
            if((next != dirInfo->d_name)) {
                char filePath[256] = "/proc/";
                strncat(filePath, dirInfo->d_name, strlen(dirInfo->d_name));
                strncat(filePath, "/comm", 5);
                file = fopen(filePath, "r");
		if(file != NULL) {
                bytes_read = fread(buffer, 1, sizeof(buffer), file);
                fclose(file);
		}
		buffer[bytes_read] = '\0';
		if(strcmp(buffer, cmdnm) == 0) {
		  printf("%s\n", dirInfo->d_name);
		}
	    }
        }
        closedir(dir);
    } else {
        printf("Unable to open /proc/ directory.\n");
        return 1;
    }
    return 0;
}

/**
 * int help()
 * - help prints out a list of valid commands, their usages, and a small
 * - description for each command to stdout.
 * Inputs: None
 * Outputs: stdout: information about valid commands
 *          0 if succesful
**/
int help() {
    printf("There's no help for you....\n");
    return 1;
}
