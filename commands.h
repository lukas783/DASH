/********************************************************************
 * commands.h
 * - A header file for prototypes and includes for commands.c
*********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <dirent.h>

/** Function Prototypes **/
int systat();
int pid(char* cmdnm);
int cmdnm(char* pid);
int help();
