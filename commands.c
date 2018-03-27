/********************************************************************
 * commands.c
 * - A file of helper functions to handle given commands
 * 
 * NOTE: The file reading portions of these subfunctions use some methods
 *       for reading and checking for errors that came from a code listing
 *       from "Advanced Linux Programming" by CodeSourcery LLC
 *       See their COPYRIGHT for more information.
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
    FILE *file;        // A file pointer 
    char buffer[4096]; // A buffer to hold read data from the file pointer
    size_t bytes_read; // A indicator for number of bytes read
    char* starting;    // A substring for holding cpuinfo from vendor_id on down
    char* ending;      // A substring for holding cpuinfo from physical id on down

    /** Open, read, and save to a buffer /proc/version for version information **/
    file = fopen("/proc/version", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(file == NULL || bytes_read == 0 || bytes_read == sizeof(buffer)) {
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
    if(file == NULL || bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/uptime is too big or nonexistent.\n");
        return 2;
    }
    /** Append a null terminator '\0' and output version info **/
    buffer[bytes_read] = '\0';
    printf("uptime:\n%s\n", buffer);

    /** Open, read, and save to a buffer /proc/meminfo for memory information **/
    file = fopen("/proc/meminfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(file == NULL || bytes_read == 0 || bytes_read == sizeof(buffer)) {
        printf("/proc/meminfo is too big or nonexistent.\n");
        return 3;
    }
    /** Append a null terminator '\0' and output version info **/
    buffer[bytes_read] = '\0';
    printf("meminfo:\n%s\n", buffer);
    

    /** We will need a substring for vendor_id to end of buffer
        and a substring for physical id to end of buffer **/ 
    /** Open, read, and save to a buffer /proc/cpuinfo for cpu information **/
    file = fopen("/proc/cpuinfo", "r");
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);
    /** If file is empty or file exceeds buffer size, error out and exit.. **/
    if(file == NULL || bytes_read == 0 || bytes_read == sizeof(buffer)) {
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
 *          2 if input string is NULL
**/
int cmdnm(char* pid) {
    FILE *file;                   // a file pointer for reading a file
    size_t bytes_read = 0;        // an int indicating how many bytes were read from file
    char buffer[512];             // a buffer to hold the read data from file
    char filePath[512] = "/proc/";// a filePath to open once appended

    /** If input string is null, exit before we break anything **/
    if(pid == NULL) {
      printf("Process ID '' does not match any running process names.\n");
      return 2;
    }
    /** If our input string ends with a \n, replace with a \0 **/
    if(pid[strlen(pid)-1] == '\n') {
      pid[strlen(pid)-1] = '\0';
    }

    /** Append our input string and /comm to our filePath variable **/
    strncat(filePath, pid, strlen(pid));
    strncat(filePath, "/comm", 5);

    /** Open our filePath and attempt to read into buffer the contents if we can **/
    file = fopen(filePath, "r");
    if(file == NULL) {
      printf("Process ID '%s' does not match any running process names.\n", pid);
      return 1;
    }
    bytes_read = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    /** Replace the end of buffer with a null terminator and output the contents **/
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
    /** If our input is null, exit before we break anything..  **/
    if(cmdnm == NULL) {
      printf("Process name '' does not match any running process IDs.\n");
      return 0;
    }
    DIR *dir;                  // a directory pointer
    long pid;                  // a long for converting filepath to long
    char *next;                // a string to verify the filepath is only an integer
    struct dirent *dirInfo;    // a default info for directory contents
    FILE *file;                // a file pointer
    char buffer[512];          // a buffer to read a file into
    size_t bytes_read;         // the read number of bytes of our file pointer
    int found = 0;             // a counter for whether we found any valid pids

    /** If our input string ends with a \n, replace it with a \0 **/
    if(cmdnm[strlen(cmdnm)-1] == '\n') {
      cmdnm[strlen(cmdnm)-1] = '\0';
    }

    /** Open the directory, if we cant print an error and return 1 **/
    if(dir=opendir("/proc/")) {
        /** Loop through all contents of the directory **/
        while((dirInfo = readdir(dir)) != NULL) {
	    /** Convert the directory name to a long **/
	    pid = strtol(dirInfo->d_name, &next, 10);
	    /** If the subdirectory is a valid long, search the directory, otherwise dont **/
            if((next != dirInfo->d_name)) {
	        /** Create a filepath to /proc/PID/comm **/
                char filePath[256] = "/proc/";
                strncat(filePath, dirInfo->d_name, strlen(dirInfo->d_name));
                strncat(filePath, "/comm", 5);

		/** Open and read the comm file into buffer if it exists.**/
                file = fopen(filePath, "r");
		if(file != NULL) {
                bytes_read = fread(buffer, 1, sizeof(buffer), file);
                fclose(file);
		}
		/** Replace the end character with \0 and compare process name to given string **/
		buffer[bytes_read-1] = '\0';
		if(strcmp(buffer, cmdnm) == 0) {
		  found += 1;
		  printf("%s\n", dirInfo->d_name);
		}
	    }
        }
        closedir(dir);
	/** If no matching process names were found, output a message to let them know **/
	if(found == 0) {
	  printf("Process name '%s' does not match any running process IDs.\n", cmdnm);
	}
    } else {
        printf("Unable to open /proc/ directory.\n");
        return 1;
    }
    return 0;
}

/**
 * int cd(char *path)
 * - cd is a function that runs the chdir function to change the working
 * - directory given a valid path. If the given path is invalid, chdir will
 * - fail and perror will output a message explaining why.
 * Inputs: char *path - the relative or absolute path to change to
 * Outputs: 0 if successful directory change
 *          -1 if the directory could not switch
**/
int cd(char *path) {
  /** Try to chdir to path**/
  if(chdir(path) == 0) {
    return 0;
  }
  /** If chdir couldn't complete, give a message as to why **/
  perror("Unable to change directories ");
  return -1;
}

/**
 * int sig(char **args)
 * - sig handles sending a signal number to a specific process. The signal
 * - number is passed as the first argument and the pid to send it to is 
 * - given as the second argument. The function validates the two arguments
 * - and then runs the kill function to send the signal if it can. If any
 * - issues come up a -1 is returned out, else a 0 is returned.
 * Inputs: char **args - a list of command arguments
 * Outputs: 0 if successful
 *          -1 if the input is invalid or kill could not run
**/
int sig(char **args) {
  /** Create two int variables and convert our command arguments to ints**/
  int arg0, arg1;
  printf("%s!\n", args[0]);
  printf("%s!!\n", args[1]);
  if(toInt(args[0], &arg0) == -1) {
    printf("Invalid signal value, signal value must be numeric.\n");
    exit(5);
  }
  if(toInt(args[1], &arg1) == -1) {
    printf("Invalid pid value, pid value must be numeric.\n");
    printf("%s.\n", args[1]);
    exit(5);
  }
  
  /** Take our freshly converted integers and run the kill function to send a signal **/
  if(kill(arg1, arg0) == 0) {
    return 0;
  }
  /** If the kill function failed, perror out and return a -1 **/
  perror("Unable to send signal to process ");
  return -1;
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
    printf("\n");
    /** Output help for cmdnm() **/
    printf("cmndm [process_id]\n - Outputs the process name for a given process ID.\n \
[process_id] - REQUIRED: replace with a valid process id.\n - Any subsequent arguments given will be ignored.\n\n");

    /** Output help for pid() **/
    printf("pid [process_name]\n - Outputs any process ID who's process name matches the given [process_name] input string.\n \
[process_name] - REQUIRED: replace with a valid process name.\n - Any subsequent arguments given will be ignored.\n\n");

    /** Output help for systat() **/
    printf("systat \n - Prints out the Linux version, the uptime of the system, the system memory info, and information about the cpu.\n \
- Any supplied arguments will be ignored.\n\n");

    /** Output signal help **/
    printf("signal [signal_number] [process_id]\n - Sends [signal_number] to the given [process_id]. For a list of valid signals, use command\
     'man -s7 signal'\n - WARNING: if the signal call is used to send a signal to the self process, undefined behavior will occur. \n   Any signal \
received by the program lower than signal 17 will terminate the program to avoid this behavior. \n - Any extra arguments will be ignored\n\n");

     /** Output CD help **/
     printf("cd [path]\n - Changes the active working directory, upon completion of the command no output should be seen if the directory change \
     was successful, there will only be output if the directory could not be changed. \n - [path] can be both a relative path or an absolute path\n \
     - Any extra arguments will be ignored\n\n");

    /** Output help for help() **/
    printf("help\n - Displays this prompt and outputs information about valid commands.\n \
- Any sebsequent arguments will be ignored.\n\n");

    
    /** Indicate that exit leaves the shell **/
    printf("exit\n - Exits the diagnostics shell program.\n - Any subsequent arguments will be ignored.\n\n");
    
    /** Output a notice that any program or script in the 'PATH' will be available as well **/
    printf("Any other command given will search through the PATH environment variable for a valid program/script and execute if possible.\n\n");
    
    return 0;
}

/**
 * int toInt(char* str, int *val)
 * - toInt converts a given input string from a c string to an integer, 
 * - given the input string contains a valid integer and only a valid integer.
 * - The output is given through a pass by reference argument and the validity
 * - of the input string is given by the function's return statement.
 * Inputs: None
 * Outputs: *val - a pass by reference integer containing a converted int
 *          0 if the string is valid
 *          -1 if the string contains an invalid character (non-integer)
**/
int toInt(char* str, int *val) {
  /** If we are given a bad input, get out before we SIGSEGV **/
  if(str == NULL)
    return -1;
  /** Get our strings length and create a sum variable **/
  int len = strlen(str);
  int sum = 0;
  /** Loop through the string, validate the character as a digit, and append as integer to sum **/
  for(int i = 0; i < len; i++) {
    if(str[i] < 48 || str[i] > 57) 
      return -1;
    sum *= 10;
    sum += str[i] - '0';
  }
  /** Replace our pass-by-reference value with our new integer and return 0 **/
  *val = sum;
  return 0;
}
