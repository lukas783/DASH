/********************************************************************
 * commands.h
 * - A header file for prototypes and includes for commands.c
*********************************************************************/
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>
#include <unistd.h>
#include <sys/wait.h>

/** Function Prototypes **/
int systat();
int pid(char* cmdnm);
int cmdnm(char* pid);
int cd(char* path);
int sig();
int help();
int toInt(char *str, int *val);
