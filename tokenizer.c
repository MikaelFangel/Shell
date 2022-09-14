#include <stdio.h>
#include <string.h>

char *main(void) {
    char command[256] = {0}, buf[256] = {0};
    fgets(command, sizeof(buf), stdin);
    
    char * token;
    char * args[256]; 
    args[0] = strtok(command, " ");

    int i = 1;
    while((args[i++] = strtok(NULL, " ")) != NULL);

    return *args;
}
