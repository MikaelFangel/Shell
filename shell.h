#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <stdbool.h>

void parser(int argc, char *argv[]);
void newProcess(char* argv[]);
void pipeProcesses(char *argvfrom[], char *argvto[]);
void pipeLine(char **args[], int count);
void changeDir(char* path);
void welcomeMsg();
void addHistory(char *argv[]);
void readHistory();